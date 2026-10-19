// ------------------------------------------------------------
//  /item: tienda (Antidoto, Virus) y cambio de skin (con la galeria)
// ------------------------------------------------------------

// Opciones del menu, en el orden en que aparecen
#define ITEM_ANTIDOTO           0
#define ITEM_VIRUS              1
#define ITEM_SKIN               2

// Espera entre cambios de skin con /item
#define ESPERA_CAMBIO_SKIN_MS   (3 * 60 * 1000)     // 3 minutos

// Momento del ultimo cambio; se guarda junto al contador de conexion para que
// no pase de un jugador al siguiente que use el mismo id
static gUltimoCambioSkin[MAX_PLAYERS];
static gConexionCambioSkin[MAX_PLAYERS] = { -1, ... };

// Milisegundos que faltan para poder cambiar de skin (0 = ya puede)
static EsperaCambioSkin(playerid)
{
    if (gConexionCambioSkin[playerid] != gContadorConsulta[playerid]) return 0;
    new pasado = GetTickCount() - gUltimoCambioSkin[playerid];
    return (pasado >= ESPERA_CAMBIO_SKIN_MS) ? 0 : (ESPERA_CAMBIO_SKIN_MS - pasado);
}

// Devuelve 1 si el comando era de este modulo
Items_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/item", true) && strcmp(comando, "/items", true)) return 0;

    if (!gJugador[playerid][jEnJuego] || Galeria_EstaAbierta(playerid)) return 1;
    MostrarMenuItems(playerid);
    return 1;
}

static MostrarMenuItems(playerid)
{
    new antidoto[20], virus[20], dinero[20], titulo[48], cuerpo[256];
    FormatearDinero(PRECIO_ANTIDOTO, antidoto);
    FormatearDinero(PRECIO_VIRUS, virus);
    FormatearDinero(Dinero_Obtener(playerid), dinero);

    format(titulo, sizeof(titulo), "Items  -  Tu dinero: %s", dinero);
    format(cuerpo, sizeof(cuerpo),
        "Item\tPrecio\tEfecto\n\
        " EMBED_HUMANO "Antidoto\t{FFFFFF}%s\t{AAAAAA}Te cura si estas infectado\n\
        {FF4040}Virus\t{FFFFFF}%s\t{AAAAAA}Te convierte en zombie\n\
        {FFFFFF}Skin\t{FFFFFF}Gratis\t{AAAAAA}Cambia tu skin",
        antidoto, virus);
    ShowPlayerDialog(playerid, DIALOGO_ITEMS, DIALOG_STYLE_TABLIST_HEADERS, titulo, cuerpo, "Elegir", "Cerrar");
}

// Devuelve 1 si el dialogo era de este modulo
Items_AlResponderDialogo(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOGO_ITEMS) return 0;
    if (!response || !gJugador[playerid][jEnJuego]) return 1;

    switch (listitem)
    {
        case ITEM_ANTIDOTO: ComprarAntidoto(playerid);
        case ITEM_VIRUS:    ComprarVirus(playerid);
        case ITEM_SKIN:     AbrirCambioSkin(playerid);
    }
    return 1;
}

static AbrirCambioSkin(playerid)
{
    new espera = EsperaCambioSkin(playerid);
    if (espera > 0)
    {
        new segundos = (espera + 999) / 1000, mensaje[80];
        format(mensaje, sizeof(mensaje), "Podes volver a cambiar de skin en %d:%02d.", segundos / 60, segundos % 60);
        SendClientMessage(playerid, COLOR_ERROR, mensaje);
        return;
    }
    Galeria_Abrir(playerid, GALERIA_SKINS);
}

static ComprarAntidoto(playerid)
{
    if (gJugador[playerid][jBando] == BANDO_HUMANO)
    {
        SendClientMessage(playerid, COLOR_ERROR, "El antidoto solo sirve si estas infectado.");
        return;
    }
    if (Nemesis_Es(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "El antidoto no funciona en Nemesis.");
        return;
    }
    if (!Dinero_Cobrar(playerid, PRECIO_ANTIDOTO, "el antidoto")) return;

    Bando_Curar(playerid);
    if (EsCEO(playerid)) Admin_AlSpawnear(playerid);
}

static ComprarVirus(playerid)
{
    if (gJugador[playerid][jBando] == BANDO_ZOMBIE)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ya estas infectado.");
        return;
    }
    if (!Dinero_Cobrar(playerid, PRECIO_VIRUS, "el virus")) return;

    Bando_Infectar(playerid);
    if (EsCEO(playerid)) Admin_AlSpawnear(playerid);
}

// Skin elegido en la galeria (solo muestra skins del bando del jugador)
Items_ElegirSkin(playerid, skin)
{
    gJugador[playerid][jSkin] = skin;
    // Si es humano, es el skin al que vuelve si lo curan
    if (gJugador[playerid][jBando] == BANDO_HUMANO) gJugador[playerid][jSkinSeleccion] = skin;
    SetPlayerSkin(playerid, skin);

    gUltimoCambioSkin[playerid] = GetTickCount();
    gConexionCambioSkin[playerid] = gContadorConsulta[playerid];

    new mensaje[80];
    format(mensaje, sizeof(mensaje), "Cambiaste tu skin al %d. Podes volver a cambiarlo en 3 minutos.", skin);
    SendClientMessage(playerid, COLOR_EXITO, mensaje);
}
