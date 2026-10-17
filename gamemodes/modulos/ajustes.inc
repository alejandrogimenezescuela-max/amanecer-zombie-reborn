// ------------------------------------------------------------
//  /config: ajustes personales de cada jugador (clima, hora y
//  mensajes/elementos que quiere ver). Se guardan en su cuenta.
// ------------------------------------------------------------

// Filas del menu principal, en orden
#define AJUSTE_CLIMA            0
#define AJUSTE_HORA             1
#define AJUSTE_LOGO             2
#define AJUSTE_RADIO            3
#define AJUSTE_AVISOS           4
#define AJUSTE_MUSICA           5

// Climas elegibles: id de clima de SA (-1 = el del servidor) y nombre
static const gClimas[] = { -1, 1, 4, 8, 19, 20 };
static const gNombresClima[][] = { "Niebla (servidor)", "Soleado", "Nublado", "Tormenta", "Tormenta de arena", "Neblina verde" };

// "Activado" en verde o "Desactivado" en rojo, para el menu
#define TextoEstado(%0,%1)      (Ajustes_Activado(%0, %1) ? ("{00CC00}Activado") : ("{FF4040}Desactivado"))

// Horas elegibles (-1 = la del servidor)
static const gHoras[] = { -1, 6, 12, 19, 0 };
static const gNombresHora[][] = { "Del servidor", "Amanecer (6:00)", "Mediodia (12:00)", "Atardecer (19:00)", "Noche (0:00)" };

// Columnas de users para guardar los ajustes
Ajustes_PrepararTabla()
{
    MySQL_AsegurarColumna("users", "cfg_clima", "TINYINT NOT NULL DEFAULT -1");
    MySQL_AsegurarColumna("users", "cfg_hora", "TINYINT NOT NULL DEFAULT -1");
    MySQL_AsegurarColumna("users", "cfg_flags", "INT NOT NULL DEFAULT 0");
}

// true si el jugador tiene activada la opcion (CFG_SIN_*)
Ajustes_Activado(playerid, opcion)
{
    return !(gJugador[playerid][jCfgFlags] & opcion);
}

// Clima y hora elegidos (o los del servidor); se llama al spawnear y al cambiarlos
Ajustes_Aplicar(playerid)
{
    SetPlayerWeather(playerid, (gJugador[playerid][jCfgClima] == -1) ? CLIMA_NIEBLA : gJugador[playerid][jCfgClima]);
    SetPlayerTime(playerid, (gJugador[playerid][jCfgHora] == -1) ? HORA_SERVIDOR : gJugador[playerid][jCfgHora], 0);
}

// Devuelve 1 si el comando era de este modulo
Ajustes_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/config", true) && strcmp(comando, "/ajustes", true)) return 0;

    if (gJugador[playerid][jEnJuego]) MostrarMenu(playerid);
    return 1;
}

static MostrarMenu(playerid)
{
    new cuerpo[512];
    format(cuerpo, sizeof(cuerpo),
        "Opcion\tActual\n\
        Clima\t{FFFFFF}%s\n\
        Hora\t{FFFFFF}%s\n\
        Logo en pantalla\t%s\n\
        Mensajes de radio\t%s\n\
        Avisos de entrada y salida\t%s\n\
        Musica del selector de skin\t%s",
        gNombresClima[IndiceDe(gClimas, sizeof(gClimas), gJugador[playerid][jCfgClima])],
        gNombresHora[IndiceDe(gHoras, sizeof(gHoras), gJugador[playerid][jCfgHora])],
        TextoEstado(playerid, CFG_SIN_LOGO),
        TextoEstado(playerid, CFG_SIN_RADIO),
        TextoEstado(playerid, CFG_SIN_AVISOS),
        TextoEstado(playerid, CFG_SIN_MUSICA));
    ShowPlayerDialog(playerid, DIALOGO_CONFIG, DIALOG_STYLE_TABLIST_HEADERS, "Configuracion", cuerpo, "Cambiar", "Cerrar");
}

// Posicion de 'valor' en la lista (0 si no esta)
static IndiceDe(const lista[], largo, valor)
{
    for (new i = 0; i < largo; i++)
    {
        if (lista[i] == valor) return i;
    }
    return 0;
}

static MostrarLista(playerid, dialogo, const titulo[], const nombres[][], cantidad)
{
    new cuerpo[256];
    for (new i = 0; i < cantidad; i++)
    {
        strcat(cuerpo, nombres[i]);
        strcat(cuerpo, "\n");
    }
    ShowPlayerDialog(playerid, dialogo, DIALOG_STYLE_LIST, titulo, cuerpo, "Elegir", "Volver");
}

// Devuelve 1 si el dialogo era de este modulo
Ajustes_AlResponderDialogo(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOGO_CONFIG && dialogid != DIALOGO_CONFIG_CLIMA && dialogid != DIALOGO_CONFIG_HORA) return 0;
    if (!gJugador[playerid][jEnJuego]) return 1;

    if (dialogid == DIALOGO_CONFIG)
    {
        if (!response) return 1;
        switch (listitem)
        {
            case AJUSTE_CLIMA:  { MostrarLista(playerid, DIALOGO_CONFIG_CLIMA, "Clima", gNombresClima, sizeof(gNombresClima)); return 1; }
            case AJUSTE_HORA:   { MostrarLista(playerid, DIALOGO_CONFIG_HORA, "Hora", gNombresHora, sizeof(gNombresHora)); return 1; }
            case AJUSTE_LOGO:
            {
                gJugador[playerid][jCfgFlags] ^= CFG_SIN_LOGO;
                Interfaz_Mostrar(playerid);
            }
            case AJUSTE_RADIO:  gJugador[playerid][jCfgFlags] ^= CFG_SIN_RADIO;
            case AJUSTE_AVISOS: gJugador[playerid][jCfgFlags] ^= CFG_SIN_AVISOS;
            case AJUSTE_MUSICA: gJugador[playerid][jCfgFlags] ^= CFG_SIN_MUSICA;
        }
        MostrarMenu(playerid);
        return 1;
    }

    // Listas de clima y hora: "Volver" regresa al menu sin cambiar nada
    if (response)
    {
        if (dialogid == DIALOGO_CONFIG_CLIMA && 0 <= listitem < sizeof(gClimas)) gJugador[playerid][jCfgClima] = gClimas[listitem];
        if (dialogid == DIALOGO_CONFIG_HORA && 0 <= listitem < sizeof(gHoras)) gJugador[playerid][jCfgHora] = gHoras[listitem];
        Ajustes_Aplicar(playerid);
    }
    MostrarMenu(playerid);
    return 1;
}
