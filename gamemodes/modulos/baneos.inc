// ------------------------------------------------------------
//  Baneos y expulsiones.
//  El ban es permanente y doble: se guarda la IP y el serial de la PC
//  (GPCI) en la tabla banlist, y al conectar se rechaza a cualquiera que
//  coincida en alguno de los dos (o en el nombre de la cuenta).
//  Tambien se guarda su Discord: despues del login, discord.inc rechaza
//  cualquier cuenta vinculada a un Discord baneado.
//  No se usa Ban() de open.mp (bans.json): todo queda en MySQL.
// ------------------------------------------------------------

#define LARGO_RAZON             64
#define LARGO_GPCI              41

forward AlRevisarBan(playerid, contador);

// Antes de pedir la clave se revisa si esta baneado; si no lo esta,
// sigue el login normal
Baneos_AlConectar(playerid)
{
    new nombre[MAX_PLAYER_NAME + 1], ip[16], serial[LARGO_GPCI], consulta[320];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    GetPlayerIp(playerid, ip, sizeof(ip));
    gpci(playerid, serial, sizeof(serial));

    mysql_format(gMySQL, consulta, sizeof(consulta),
        "SELECT motivo, admin_nombre FROM banlist \
         WHERE activo = 1 AND (expira IS NULL OR expira > NOW()) \
         AND (ip = '%e' OR gpci = '%e' OR nombre = '%e') LIMIT 1",
        ip, serial, nombre);
    mysql_tquery(gMySQL, consulta, "AlRevisarBan", "dd", playerid, gContadorConsulta[playerid]);
}

public AlRevisarBan(playerid, contador)
{
    if (contador != gContadorConsulta[playerid]) return 1;

    if (cache_num_rows() == 0)
    {
        Cuentas_AlConectar(playerid);
        return 1;
    }

    new motivo[LARGO_RAZON + 1], admin[MAX_PLAYER_NAME + 1], mensaje[144];
    cache_get_value_name(0, "motivo", motivo, sizeof(motivo));
    cache_get_value_name(0, "admin_nombre", admin, sizeof(admin));

    SendClientMessage(playerid, COLOR_ERROR, "Estas baneado permanentemente de este servidor.");
    format(mensaje, sizeof(mensaje), "Razon: %s (baneado por %s)", motivo, admin);
    SendClientMessage(playerid, COLOR_ERROR, mensaje);
    Conexion_MarcarBan(playerid);
    ExpulsarConRetraso(playerid);
    return 1;
}

Baneos_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (!strcmp(comando, "/ban", true)) return ComandoSancion(playerid, cmdtext, true);
    if (!strcmp(comando, "/kick", true)) return ComandoSancion(playerid, cmdtext, false);
    return 0;
}

// /ban [id] [razon] (administrador y CEO) y /kick [id] [razon] (desde moderador).
// Se puede usar sobre uno mismo, pero no sobre alguien de rango mayor.
static ComandoSancion(playerid, const cmdtext[], bool:esBan)
{
    if (Admin_Rango(playerid) < (esBan ? RANGO_ADMINISTRADOR : RANGO_MODERADOR))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }

    new pid[12], razon[LARGO_RAZON + 1];
    ObtenerParametro(cmdtext, 1, pid);
    ObtenerResto(cmdtext, 2, razon);
    if (pid[0] == EOS || razon[0] == EOS)
    {
        SendClientMessage(playerid, COLOR_ERROR, esBan ? ("Uso: /ban [id] [razon]") : ("Uso: /kick [id] [razon]"));
        return 1;
    }

    new objetivo = strval(pid);
    if (!IsPlayerConnected(objetivo))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ese jugador no esta conectado.");
        return 1;
    }
    if (objetivo != playerid && Admin_Rango(objetivo) > Admin_Rango(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No podes sancionar a alguien de rango mayor.");
        return 1;
    }

    new nombre[MAX_PLAYER_NAME + 1], admin[MAX_PLAYER_NAME + 1], mensaje[144];
    GetPlayerName(objetivo, nombre, sizeof(nombre));
    GetPlayerName(playerid, admin, sizeof(admin));

    if (esBan)
    {
        new ip[16], serial[LARGO_GPCI], consulta[512];
        GetPlayerIp(objetivo, ip, sizeof(ip));
        gpci(objetivo, serial, sizeof(serial));

        mysql_format(gMySQL, consulta, sizeof(consulta),
            "INSERT INTO banlist (user_id, nombre, ip, subred, gpci, discord_id, motivo, admin_nombre) \
             VALUES (NULLIF(%d, 0), '%e', '%e', SUBSTRING_INDEX('%e', '.', 3), '%e', \
             (SELECT discord_id FROM users WHERE id = %d), '%e', '%e')",
            gJugador[objetivo][jId], nombre, ip, ip, serial, gJugador[objetivo][jId], razon, admin);
        mysql_tquery(gMySQL, consulta);

        format(mensaje, sizeof(mensaje), "[Ban] %s fue baneado por %s. Razon: %s", nombre, admin, razon);
        Conexion_MarcarBan(objetivo);
    }
    else
    {
        format(mensaje, sizeof(mensaje), "[Kick] %s fue expulsado por %s. Razon: %s", nombre, admin, razon);
    }
    SendClientMessageToAll(COLOR_ERROR, mensaje);
    ExpulsarConRetraso(objetivo);
    return 1;
}

// Copia el texto desde la palabra numero 'indice' hasta el final
static ObtenerResto(const texto[], indice, destino[], largo = sizeof(destino))
{
    new pos = 0, actual = 0;
    destino[0] = EOS;

    while (texto[pos] != EOS)
    {
        while (texto[pos] == ' ') pos++;
        if (texto[pos] == EOS) break;

        if (actual == indice)
        {
            strmid(destino, texto, pos, strlen(texto), largo);
            return;
        }
        while (texto[pos] != EOS && texto[pos] != ' ') pos++;
        actual++;
    }
}
