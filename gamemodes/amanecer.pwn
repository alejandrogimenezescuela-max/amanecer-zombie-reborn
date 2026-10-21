#include <open.mp>
#include <a_mysql>
#include <streamer>

// Modulos del gamemode (gamemodes/modulos/)
#include "modulos/config.inc"
#include "modulos/utiles.inc"
#include "modulos/jugador.inc"
#include "modulos/mysql.inc"
#include "modulos/dinero.inc"
#include "modulos/bandos.inc"
#include "modulos/spawns.inc"
#include "modulos/cuentas.inc"
#include "modulos/admin.inc"
#include "modulos/fuego.inc"
#include "modulos/conexiones.inc"
#include "modulos/baneos.inc"
#include "modulos/discord.inc"
#include "modulos/galeria.inc"
#include "modulos/items.inc"
#include "modulos/armas.inc"
#include "modulos/autos.inc"
#include "modulos/efectos.inc"
#include "modulos/nemesis.inc"
#include "modulos/vuelo.inc"
#include "modulos/interfaz.inc"
#include "modulos/parque.inc"
#include "modulos/mapeos.inc"
#include "modulos/ajustes.inc"
#include "modulos/perfil.inc"

main()
{
    print("Amanecer Zombie Reborn - gamemode cargado");
}

public OnGameModeInit()
{
    MySQL_Conectar();
    Dinero_PrepararTabla();
    Ajustes_PrepararTabla();
    Discord_PrepararTabla();
    SetGameModeText("Zombie Survival");
    SetWeather(CLIMA_NIEBLA);
    UsePlayerPedAnims();    // todos los skins caminan y corren como CJ
    Bando_CrearClases();
    Spawns_Cargar();
    Admin_Iniciar();
    Fuego_Iniciar();
    Interfaz_Crear();
    Parque_Iniciar();
    Nemesis_Iniciar();
    Mapeos_Cargar();
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
    Nemesis_AlConectar(playerid);
    Discord_AlConectar(playerid);
    Mapeos_QuitarEdificios(playerid);
    gContadorConsulta[playerid]++;

    SendClientMessage(playerid, COLOR_EXITO, "Bienvenido a Amanecer Zombie Reborn.");
    Conexion_AlEntrar(playerid);

    Bando_CrearTexto(playerid);
    Baneos_AlConectar(playerid);   // si no esta baneado sigue con el login
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Conexion_AlSalir(playerid, reason);
    Galeria_Cerrar(playerid);
    Vuelo_Desactivar(playerid);
    Fuego_AlSalir(playerid);
    Autos_AlSalir(playerid);
    Parque_AlSalir(playerid);
    Nemesis_Terminar(playerid);
    GuardarCuenta(playerid);
    gContadorConsulta[playerid]++;
    ReiniciarDatos(playerid);
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (Cuentas_AlResponderDialogo(playerid, dialogid, response, inputtext)) return 1;
    if (Discord_AlResponderDialogo(playerid, dialogid, response)) return 1;
    if (Spawns_AlResponderDialogo(playerid, dialogid, response, listitem)) return 1;
    if (Items_AlResponderDialogo(playerid, dialogid, response, listitem)) return 1;
    if (Armas_AlResponderDialogo(playerid, dialogid, response, listitem)) return 1;
    if (Nemesis_AlResponderDialogo(playerid, dialogid, response)) return 1;
    if (Ajustes_AlResponderDialogo(playerid, dialogid, response, listitem)) return 1;
    return 0;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if (!gJugador[playerid][jLogueado]) return 1;
    if (Spawns_AlComando(playerid, cmdtext)) return 1;
    if (Admin_AlComando(playerid, cmdtext)) return 1;
    if (Baneos_AlComando(playerid, cmdtext)) return 1;
    if (Fuego_AlComando(playerid, cmdtext)) return 1;
    if (Items_AlComando(playerid, cmdtext)) return 1;
    if (Armas_AlComando(playerid, cmdtext)) return 1;
    if (Autos_AlComando(playerid, cmdtext)) return 1;
    if (Vuelo_AlComando(playerid, cmdtext)) return 1;
    if (Parque_AlComando(playerid, cmdtext)) return 1;
    if (Ajustes_AlComando(playerid, cmdtext)) return 1;
    if (Perfil_AlComando(playerid, cmdtext)) return 1;
    if (Efectos_AlComando(playerid, cmdtext)) return 1;
    if (Nemesis_AlComando(playerid, cmdtext)) return 1;
    return 0;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    if (Galeria_AlClicTextDraw(playerid, playertextid)) return 1;
    return 0;
}

public OnPlayerUpdate(playerid)
{
    Fuego_AlActualizar(playerid);
    Vuelo_AlActualizar(playerid);
    return 1;   // devolver 0 bloquearia la sincronizacion del jugador
}

public OnPlayerEditAttachedObject(playerid, EDIT_RESPONSE:response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ)
{
    if (Fuego_AlEditarObjeto(playerid, response, index, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ)) return 1;
    Nemesis_AlEditarObjeto(playerid, response, index, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
    return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
    // INVALID_TEXT_DRAW = el jugador apreto ESC durante la seleccion
    if (clickedid == INVALID_TEXT_DRAW) Galeria_AlCancelarSeleccion(playerid);
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
    Vuelo_Desactivar(playerid);
    Vuelo_PrecargarAnimaciones(playerid);
    Interfaz_Mostrar(playerid);
    Ajustes_Aplicar(playerid);
    Parque_AlSpawnear(playerid);
    return 1;
}

public OnPlayerText(playerid, text[])
{
    // Sin sesion no se habla; con sesion el mensaje sale con el bando
    if (gJugador[playerid][jLogueado]) Bando_AlHablar(playerid, text);
    return 0;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    if (!gJugador[playerid][jLogueado]) return 1;

    Galeria_Cerrar(playerid);
    Vuelo_Desactivar(playerid);
    Efectos_AlMorir(playerid);
    Cuentas_AlMorir(playerid);
    Nemesis_AlMorir(playerid);
    Bando_AlMorir(playerid);
    return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
    Autos_AlMorirVehiculo(vehicleid);
    return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
    Parque_AlEntrarCheckpoint(playerid);
    return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
    Nemesis_AlTocarPickup(playerid, pickupid);
    return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, WEAPON:weaponid, bodypart)
{
    Nemesis_AlRecibirDanio(playerid, amount);
    return 1;
}
