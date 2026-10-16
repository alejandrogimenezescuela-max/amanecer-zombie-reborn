// ------------------------------------------------------------
//  Admins RCON: skin propio
// ------------------------------------------------------------

forward AplicarSkinAdmin(playerid, contador);

// OnRconLoginAttempt solo da la IP: se revisa un instante mas tarde a los
// jugadores con esa IP (para entonces IsPlayerAdmin ya esta activo).
Admin_AlLoginRcon(const ip[], success)
{
    if (!success) return;

    new ipJugador[16];
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i)) continue;

        GetPlayerIp(i, ipJugador, sizeof(ipJugador));
        if (!strcmp(ip, ipJugador))
        {
            SetTimerEx("AplicarSkinAdmin", 200, false, "dd", i, gContadorConsulta[i]);
        }
    }
}

public AplicarSkinAdmin(playerid, contador)
{
    if (contador != gContadorConsulta[playerid] || !IsPlayerAdmin(playerid)) return 1;

    gJugador[playerid][jSkin] = SKIN_ADMIN;
    if (gJugador[playerid][jEnJuego]) SetPlayerSkin(playerid, SKIN_ADMIN);
    return 1;
}

// Al spawnear (tambien despues de morir) el admin conserva su skin
Admin_AlSpawnear(playerid)
{
    if (!IsPlayerAdmin(playerid)) return;

    gJugador[playerid][jSkin] = SKIN_ADMIN;
    SetPlayerSkin(playerid, SKIN_ADMIN);
}

// Devuelve 1 si el comando era de este modulo
Admin_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    new bool:esCura = !strcmp(comando, "/cure", true);
    if (!esCura && strcmp(comando, "/virus", true)) return 0;

    if (!IsPlayerAdmin(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo los administradores pueden usar este comando.");
        return 1;
    }

    // /cure [id] y /virus [id]: sin id se aplica a uno mismo
    new pid[12], objetivo = playerid;
    ObtenerParametro(cmdtext, 1, pid);
    if (pid[0] != EOS) objetivo = strval(pid);

    if (!IsPlayerConnected(objetivo) || !gJugador[objetivo][jEnJuego])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ese jugador no esta en juego.");
        return 1;
    }
    if (esCura)
    {
        if (gJugador[objetivo][jBando] == BANDO_HUMANO)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Ese jugador ya es humano.");
            return 1;
        }
        Bando_Curar(objetivo);
    }
    else
    {
        if (gJugador[objetivo][jBando] == BANDO_ZOMBIE)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Ese jugador ya es zombie.");
            return 1;
        }
        Bando_Infectar(objetivo);
    }

    if (IsPlayerAdmin(objetivo)) Admin_AlSpawnear(objetivo);
    return 1;
}
