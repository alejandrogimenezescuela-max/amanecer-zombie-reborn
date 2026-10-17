// ------------------------------------------------------------
//  Avisos de entrada y salida de jugadores
// ------------------------------------------------------------

// Motivos que recibe OnPlayerDisconnect
#define MOTIVO_CRASH            0       // timeout / crash
#define MOTIVO_SALIO            1       // /q o cerro el juego
#define MOTIVO_KICK             2       // Kick() o Ban(): el servidor no los distingue

// Se marca antes de llamar a Ban() para que el aviso diga "Ban" y no "Kick".
// Todavia no hay sistema de bans: lo va a usar cuando exista.
static bool:gSaleBaneado[MAX_PLAYERS];

stock Conexion_MarcarBan(playerid)
{
    gSaleBaneado[playerid] = true;
}

Conexion_AlEntrar(playerid)
{
    gSaleBaneado[playerid] = false;

    new nombre[MAX_PLAYER_NAME + 1], mensaje[80];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(mensaje, sizeof(mensaje), "[<!>] %s entro al servidor.", nombre);
    AvisarATodos(mensaje);
}

Conexion_AlSalir(playerid, motivo)
{
    new nombre[MAX_PLAYER_NAME + 1], razon[8], mensaje[96];
    GetPlayerName(playerid, nombre, sizeof(nombre));

    switch (motivo)
    {
        case MOTIVO_CRASH: razon = "Crash";
        case MOTIVO_KICK:  razon = gSaleBaneado[playerid] ? ("Ban") : ("Kick");
        default:           razon = "Salio";
    }
    gSaleBaneado[playerid] = false;

    // El que sale ya no lo ve (su juego se cerro); le llega al resto
    format(mensaje, sizeof(mensaje), "[<!>] %s salio del servidor [%s]", nombre, razon);
    AvisarATodos(mensaje);
}

static AvisarATodos(const mensaje[])
{
    SendClientMessageToAll(COLOR_AVISO, mensaje);
}
