// ------------------------------------------------------------
//  /item: tienda (Antidoto, Virus) y cambio de skin (con la galeria)
// ------------------------------------------------------------

// Opciones del menu, en el orden en que aparecen
#define ITEM_ANTIDOTO           0
#define ITEM_VIRUS              1
#define ITEM_SKIN               2

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
        case ITEM_SKIN:     Galeria_Abrir(playerid, GALERIA_SKINS);
    }
    return 1;
}

static ComprarAntidoto(playerid)
{
    if (gJugador[playerid][jBando] == BANDO_HUMANO)
    {
        SendClientMessage(playerid, COLOR_ERROR, "El antidoto solo sirve si estas infectado.");
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

    new mensaje[48];
    format(mensaje, sizeof(mensaje), "Cambiaste tu skin al %d.", skin);
    SendClientMessage(playerid, COLOR_EXITO, mensaje);
}
