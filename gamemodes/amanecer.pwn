#include <open.mp>
#include <a_mysql>

// Modulos del gamemode (gamemodes/modulos/)
#include "modulos/config.inc"
#include "modulos/utiles.inc"
#include "modulos/jugador.inc"
#include "modulos/mysql.inc"
#include "modulos/bandos.inc"
#include "modulos/spawns.inc"
#include "modulos/cuentas.inc"
#include "modulos/admin.inc"

main()
{
    print("Amanecer Zombie Reborn - gamemode cargado");
}

public OnGameModeInit()
{
    MySQL_Conectar();
    SetGameModeText("Zombie Survival");
    SetWeather(CLIMA_NIEBLA);
    Bando_CrearClases();
    Spawns_Cargar();
    return 1;
}

public OnGameModeExit()
{
    Cuentas_GuardarTodos();
    MySQL_Cerrar();
    return 1;
}

public OnPlayerConnect(playerid)
{
    ReiniciarDatos(playerid);
    gContadorConsulta[playerid]++;

    SendClientMessage(playerid, COLOR_EXITO, "Bienvenido a Amanecer Zombie Reborn.");
    SendClientMessage(playerid, COLOR_INFO, "El mundo cayo. Sobrevivi como puedas...");

    Bando_CrearTexto(playerid);
    Cuentas_AlConectar(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    GuardarCuenta(playerid);
    gContadorConsulta[playerid]++;
    ReiniciarDatos(playerid);
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (Cuentas_AlResponderDialogo(playerid, dialogid, response, inputtext)) return 1;
    if (Spawns_AlResponderDialogo(playerid, dialogid, response, listitem)) return 1;
    return 0;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if (!gJugador[playerid][jLogueado]) return 1;
    if (Spawns_AlComando(playerid, cmdtext)) return 1;
    if (Admin_AlComando(playerid, cmdtext)) return 1;
    return 0;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    Spawns_AlClicEnMapa(playerid, fX, fY, fZ);
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    if (!gJugador[playerid][jLogueado])
    {
        PonerEnEspera(playerid);
        return 1;
    }

    // El skin se elige una sola vez por sesion
    if (gJugador[playerid][jEnJuego])
    {
        Bando_SaltearSelector(playerid);
        return 1;
    }
    Bando_MostrarSkin(playerid, classid);
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    // Nadie spawnea sin iniciar sesion
    if (!gJugador[playerid][jLogueado]) return 0;

    if (!gJugador[playerid][jEnJuego]) Bando_ConfirmarSeleccion(playerid);
    return 1;
}

public OnPlayerSpawn(playerid)
{
    if (!gJugador[playerid][jLogueado])
    {
        PonerEnEspera(playerid);
        return 1;
    }

    Cuentas_AlSpawnear(playerid);
    Bando_AlSpawnear(playerid);
    Admin_AlSpawnear(playerid);
    return 1;
}

public OnPlayerText(playerid, text[])
{
    // Sin sesion no se habla; con sesion el mensaje sale con el bando
    if (gJugador[playerid][jLogueado]) Bando_AlHablar(playerid, text);
    return 0;
}

public OnRconLoginAttempt(ip[], password[], success)
{
    Admin_AlLoginRcon(ip, success);
    return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    if (!gJugador[playerid][jLogueado]) return 1;

    Cuentas_AlMorir(playerid);
    Bando_AlMorir(playerid);
    return 1;
}
