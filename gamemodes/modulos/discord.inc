// ------------------------------------------------------------
//  Vinculacion obligatoria con Discord.
//  Despues del login, si la cuenta no tiene Discord, el jugador queda en
//  espera con un codigo. En el servidor de Discord escribe /vincular CODIGO
//  y el bot (carpeta bot/) guarda su discord_id en users. Aca se consulta
//  cada pocos segundos hasta que aparece y recien ahi se lo deja jugar.
//  Un Discord vinculado a una cuenta baneada no puede entrar.
// ------------------------------------------------------------

#define LARGO_CODIGO_DISCORD    6
#define DISCORD_REVISAR_MS      4000        // cada cuanto se pregunta si ya vinculo
#define DISCORD_CODIGO_MINUTOS  10          // vencimiento del codigo

forward AlRevisarDiscord(playerid, contador);
forward RevisarDiscord(playerid, contador);

// Sin 0/O ni 1/I/L para que no se confundan al copiarlo
static const CARACTERES_CODIGO[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";

static bool:gEsperandoDiscord[MAX_PLAYERS];
static gCodigoDiscord[MAX_PLAYERS][LARGO_CODIGO_DISCORD + 1];

Discord_PrepararTabla()
{
    mysql_tquery(gMySQL,
        "CREATE TABLE IF NOT EXISTS discord_codigos ( \
            codigo  CHAR(6)      NOT NULL, \
            user_id INT UNSIGNED NOT NULL, \
            expira  DATETIME     NOT NULL, \
            PRIMARY KEY (codigo), \
            UNIQUE KEY uq_codigo_user (user_id), \
            CONSTRAINT fk_codigo_user FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE \
        ) ENGINE=InnoDB");
}

Discord_AlConectar(playerid)
{
    gEsperandoDiscord[playerid] = false;
    gCodigoDiscord[playerid][0] = EOS;
}

// Se llama cuando la clave es correcta (o recien se registro), en lugar de
// pasar directo al selector de skin
Discord_Verificar(playerid)
{
    if (gEsperandoDiscord[playerid]) return;
    gEsperandoDiscord[playerid] = true;
    gCodigoDiscord[playerid][0] = EOS;
    RevisarDiscord(playerid, gContadorConsulta[playerid]);
}

public RevisarDiscord(playerid, contador)
{
    if (contador != gContadorConsulta[playerid] || !gEsperandoDiscord[playerid]) return 1;

    new consulta[512];
    mysql_format(gMySQL, consulta, sizeof(consulta),
        "SELECT u.discord_id, \
            (SELECT COUNT(*) FROM banlist b WHERE b.activo = 1 AND (b.expira IS NULL OR b.expira > NOW()) \
             AND b.discord_id = u.discord_id) AS baneado, \
            c.codigo \
         FROM users u LEFT JOIN discord_codigos c ON c.user_id = u.id AND c.expira > NOW() \
         WHERE u.id = %d",
        gJugador[playerid][jId]);
    mysql_tquery(gMySQL, consulta, "AlRevisarDiscord", "dd", playerid, contador);
    return 1;
}

public AlRevisarDiscord(playerid, contador)
{
    if (contador != gContadorConsulta[playerid] || !gEsperandoDiscord[playerid] || cache_num_rows() == 0) return 1;

    new bool:sinDiscord, baneado;
    cache_is_value_name_null(0, "discord_id", sinDiscord);
    cache_get_value_name_int(0, "baneado", baneado);

    if (baneado)
    {
        gEsperandoDiscord[playerid] = false;
        SendClientMessage(playerid, COLOR_ERROR, "El Discord vinculado a esta cuenta esta baneado del servidor.");
        Conexion_MarcarBan(playerid);
        ExpulsarConRetraso(playerid);
        return 1;
    }

    if (!sinDiscord)
    {
        // Si estaba esperando con el codigo en pantalla, se le cierra el cartel
        if (gCodigoDiscord[playerid][0] != EOS)
        {
            ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");
            SendClientMessage(playerid, COLOR_EXITO, "Discord vinculado. Ya podes jugar.");
        }
        gEsperandoDiscord[playerid] = false;
        IniciarSesion(playerid);
        return 1;
    }

    // Sin codigo vigente (primera vez o se vencio): se genera uno nuevo
    new bool:sinCodigo;
    cache_is_value_name_null(0, "codigo", sinCodigo);
    if (sinCodigo)
    {
        GenerarCodigo(gCodigoDiscord[playerid]);

        new consulta[160];
        mysql_format(gMySQL, consulta, sizeof(consulta),
            "REPLACE INTO discord_codigos (codigo, user_id, expira) VALUES ('%e', %d, NOW() + INTERVAL %d MINUTE)",
            gCodigoDiscord[playerid], gJugador[playerid][jId], DISCORD_CODIGO_MINUTOS);
        mysql_tquery(gMySQL, consulta);
        MostrarCodigo(playerid);
    }
    else if (gCodigoDiscord[playerid][0] == EOS)
    {
        // Reconecto con un codigo todavia vigente: se le muestra el mismo
        cache_get_value_name(0, "codigo", gCodigoDiscord[playerid], LARGO_CODIGO_DISCORD + 1);
        MostrarCodigo(playerid);
    }

    SetTimerEx("RevisarDiscord", DISCORD_REVISAR_MS, false, "dd", playerid, contador);
    return 1;
}

static MostrarCodigo(playerid)
{
    new cuerpo[512];
    format(cuerpo, sizeof(cuerpo),
        "{FFFFFF}Para jugar tenes que vincular tu cuenta con Discord.\n\n\
         1. Entra a nuestro servidor de Discord.\n\
         2. Escribi este comando en cualquier canal:\n\n\
         {00FF00}/vincular %s\n\n\
         {FFFFFF}El codigo vence en %d minutos. Cuando lo hagas, entras solo.",
        gCodigoDiscord[playerid], DISCORD_CODIGO_MINUTOS);
    ShowPlayerDialog(playerid, DIALOGO_DISCORD, DIALOG_STYLE_MSGBOX, "Vincular Discord", cuerpo, "Listo", "Salir");
}

// Devuelve 1 si el dialogo era de este modulo
Discord_AlResponderDialogo(playerid, dialogid, response)
{
    if (dialogid != DIALOGO_DISCORD) return 0;
    if (!gEsperandoDiscord[playerid]) return 1;

    if (!response)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Saliste del servidor.");
        ExpulsarConRetraso(playerid);
        return 1;
    }
    // "Listo": se vuelve a mostrar hasta que el bot confirme la vinculacion
    SendClientMessage(playerid, COLOR_INFO, "Esperando que vincules la cuenta con /vincular en Discord...");
    MostrarCodigo(playerid);
    return 1;
}

static GenerarCodigo(destino[])
{
    for (new i = 0; i < LARGO_CODIGO_DISCORD; i++)
    {
        destino[i] = CARACTERES_CODIGO[random(sizeof(CARACTERES_CODIGO) - 1)];
    }
    destino[LARGO_CODIGO_DISCORD] = EOS;
}
