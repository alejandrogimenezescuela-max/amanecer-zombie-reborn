// ------------------------------------------------------------
//  /item: tienda (Antidoto, Virus) y selector de skin con imagenes
// ------------------------------------------------------------

// Opciones del menu, en el orden en que aparecen
#define ITEM_ANTIDOTO           0
#define ITEM_VIRUS              1
#define ITEM_SKIN               2

// Selector de skin: grilla de 5 x 3 vistas previas por pagina
#define SEL_COLUMNAS            5
#define SEL_FILAS               3
#define SEL_POR_PAGINA          (SEL_COLUMNAS * SEL_FILAS)

const Float:SEL_X = 125.0;              // esquina del fondo
const Float:SEL_Y = 110.0;
const Float:SEL_ANCHO = 390.0;
const Float:SEL_ALTO = 275.0;
const Float:SEL_CASILLA = 75.0;         // separacion entre vistas previas
const Float:SEL_MODELO = 70.0;          // tamano de cada vista previa

#define SEL_COLOR_FONDO         0x000000CC
#define SEL_COLOR_CASILLA       0x2A2A2AFF
#define SEL_COLOR_BOTON         0x444444FF
#define SEL_COLOR_HOVER         0x00CC00FF

static bool:gSelAbierto[MAX_PLAYERS];
static gSelPagina[MAX_PLAYERS];
static PlayerText:gSelFondo[MAX_PLAYERS];
static PlayerText:gSelTitulo[MAX_PLAYERS];
static PlayerText:gSelModelos[MAX_PLAYERS][SEL_POR_PAGINA];
static PlayerText:gSelAnterior[MAX_PLAYERS];
static PlayerText:gSelSiguiente[MAX_PLAYERS];
static PlayerText:gSelCerrar[MAX_PLAYERS];

// ------------------------------------------------------------
//  Menu /item
// ------------------------------------------------------------

// Devuelve 1 si el comando era de este modulo
Items_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/item", true) && strcmp(comando, "/items", true)) return 0;

    if (!gJugador[playerid][jEnJuego] || gSelAbierto[playerid]) return 1;
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
        case ITEM_SKIN:     AbrirSelector(playerid);
    }
    return 1;
}

// Descuenta el precio si le alcanza; si no, avisa y devuelve false
static bool:Cobrar(playerid, precio, const item[])
{
    new texto[20], mensaje[96];
    FormatearDinero(precio, texto);

    if (Dinero_Obtener(playerid) < precio)
    {
        format(mensaje, sizeof(mensaje), "No tenes dinero suficiente: %s cuesta %s.", item, texto);
        SendClientMessage(playerid, COLOR_ERROR, mensaje);
        return false;
    }

    Dinero_Dar(playerid, -precio);
    format(mensaje, sizeof(mensaje), "Compraste %s por %s.", item, texto);
    SendClientMessage(playerid, COLOR_INFO, mensaje);
    return true;
}

static ComprarAntidoto(playerid)
{
    if (gJugador[playerid][jBando] == BANDO_HUMANO)
    {
        SendClientMessage(playerid, COLOR_ERROR, "El antidoto solo sirve si estas infectado.");
        return;
    }
    if (!Cobrar(playerid, PRECIO_ANTIDOTO, "el antidoto")) return;

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
    if (!Cobrar(playerid, PRECIO_VIRUS, "el virus")) return;

    Bando_Infectar(playerid);
    if (EsCEO(playerid)) Admin_AlSpawnear(playerid);
}

// ------------------------------------------------------------
//  Selector de skin con vistas previas
// ------------------------------------------------------------

static PlayerText:CrearBoton(playerid, Float:x, Float:y, const texto[], Float:ancho)
{
    new PlayerText:boton = CreatePlayerTextDraw(playerid, x, y, texto);
    PlayerTextDrawAlignment(playerid, boton, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(playerid, boton, TEXT_DRAW_FONT_AHARONI_BOLD);
    PlayerTextDrawLetterSize(playerid, boton, 0.3, 1.3);
    PlayerTextDrawSetOutline(playerid, boton, 1);
    PlayerTextDrawColour(playerid, boton, 0xFFFFFFFF);
    PlayerTextDrawUseBox(playerid, boton, true);
    PlayerTextDrawBoxColour(playerid, boton, SEL_COLOR_BOTON);
    // Con alineacion centrada, el segundo valor es el ancho de la caja (y del area clickeable)
    PlayerTextDrawTextSize(playerid, boton, 12.0, ancho);
    PlayerTextDrawSetSelectable(playerid, boton, true);
    return boton;
}

static AbrirSelector(playerid)
{
    if (gSelAbierto[playerid]) return;
    gSelAbierto[playerid] = true;
    gSelPagina[playerid] = 0;

    // Fondo
    gSelFondo[playerid] = CreatePlayerTextDraw(playerid, SEL_X, SEL_Y, "LD_SPAC:white");
    PlayerTextDrawFont(playerid, gSelFondo[playerid], TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawTextSize(playerid, gSelFondo[playerid], SEL_ANCHO, SEL_ALTO);
    PlayerTextDrawColour(playerid, gSelFondo[playerid], SEL_COLOR_FONDO);

    // Titulo con el bando y la pagina
    gSelTitulo[playerid] = CreatePlayerTextDraw(playerid, SEL_X + SEL_ANCHO / 2.0, SEL_Y + 5.0, "_");
    PlayerTextDrawAlignment(playerid, gSelTitulo[playerid], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(playerid, gSelTitulo[playerid], TEXT_DRAW_FONT_AHARONI_BOLD);
    PlayerTextDrawLetterSize(playerid, gSelTitulo[playerid], 0.35, 1.5);
    PlayerTextDrawSetOutline(playerid, gSelTitulo[playerid], 1);
    PlayerTextDrawColour(playerid, gSelTitulo[playerid], Bando_Color(gJugador[playerid][jBando]));

    // Vistas previas de los skins
    for (new i = 0; i < SEL_POR_PAGINA; i++)
    {
        new Float:x = SEL_X + 10.0 + float(i % SEL_COLUMNAS) * SEL_CASILLA;
        new Float:y = SEL_Y + 28.0 + float(i / SEL_COLUMNAS) * SEL_CASILLA;
        new PlayerText:modelo = CreatePlayerTextDraw(playerid, x, y, "_");

        PlayerTextDrawFont(playerid, modelo, TEXT_DRAW_FONT_MODEL_PREVIEW);
        PlayerTextDrawTextSize(playerid, modelo, SEL_MODELO, SEL_MODELO);
        PlayerTextDrawUseBox(playerid, modelo, true);
        PlayerTextDrawBoxColour(playerid, modelo, SEL_COLOR_CASILLA);
        PlayerTextDrawBackgroundColour(playerid, modelo, SEL_COLOR_CASILLA);
        PlayerTextDrawSetPreviewRot(playerid, modelo, -10.0, 0.0, -20.0, 1.0);
        PlayerTextDrawSetSelectable(playerid, modelo, true);
        gSelModelos[playerid][i] = modelo;
    }

    // Botones
    new Float:yBotones = SEL_Y + SEL_ALTO - 20.0;
    gSelAnterior[playerid] = CrearBoton(playerid, SEL_X + 50.0, yBotones, "<<", 50.0);
    gSelCerrar[playerid] = CrearBoton(playerid, SEL_X + SEL_ANCHO / 2.0, yBotones, "Cerrar", 70.0);
    gSelSiguiente[playerid] = CrearBoton(playerid, SEL_X + SEL_ANCHO - 50.0, yBotones, ">>", 50.0);

    PlayerTextDrawShow(playerid, gSelFondo[playerid]);
    PlayerTextDrawShow(playerid, gSelCerrar[playerid]);
    MostrarPagina(playerid);
    SelectTextDraw(playerid, SEL_COLOR_HOVER);
}

static TotalPaginas(playerid)
{
    new total = Bando_TotalSkins(gJugador[playerid][jBando]);
    return (total + SEL_POR_PAGINA - 1) / SEL_POR_PAGINA;
}

static MostrarPagina(playerid)
{
    new E_BANDO:bando = gJugador[playerid][jBando];
    new total = Bando_TotalSkins(bando), paginas = TotalPaginas(playerid);

    for (new i = 0; i < SEL_POR_PAGINA; i++)
    {
        new indice = gSelPagina[playerid] * SEL_POR_PAGINA + i;
        if (indice < total)
        {
            PlayerTextDrawSetPreviewModel(playerid, gSelModelos[playerid][i], Bando_SkinEnIndice(bando, indice));
            PlayerTextDrawShow(playerid, gSelModelos[playerid][i]);
        }
        else
        {
            PlayerTextDrawHide(playerid, gSelModelos[playerid][i]);
        }
    }

    new nombre[8], titulo[64];
    Bando_Nombre(bando, nombre);
    format(titulo, sizeof(titulo), "Skins de %s  -  Pagina %d/%d", nombre, gSelPagina[playerid] + 1, paginas);
    PlayerTextDrawSetString(playerid, gSelTitulo[playerid], titulo);
    PlayerTextDrawShow(playerid, gSelTitulo[playerid]);

    // Las flechas solo aparecen si hay mas de una pagina
    if (paginas > 1)
    {
        PlayerTextDrawShow(playerid, gSelAnterior[playerid]);
        PlayerTextDrawShow(playerid, gSelSiguiente[playerid]);
    }
    else
    {
        PlayerTextDrawHide(playerid, gSelAnterior[playerid]);
        PlayerTextDrawHide(playerid, gSelSiguiente[playerid]);
    }
}

// Cierra el selector; 'cancelar' es false cuando el jugador ya salio con ESC
static CerrarSelector(playerid, bool:cancelar = true)
{
    if (!gSelAbierto[playerid]) return;
    gSelAbierto[playerid] = false;

    PlayerTextDrawDestroy(playerid, gSelFondo[playerid]);
    PlayerTextDrawDestroy(playerid, gSelTitulo[playerid]);
    for (new i = 0; i < SEL_POR_PAGINA; i++)
    {
        PlayerTextDrawDestroy(playerid, gSelModelos[playerid][i]);
    }
    PlayerTextDrawDestroy(playerid, gSelAnterior[playerid]);
    PlayerTextDrawDestroy(playerid, gSelSiguiente[playerid]);
    PlayerTextDrawDestroy(playerid, gSelCerrar[playerid]);

    if (cancelar) CancelSelectTextDraw(playerid);
}

// Devuelve 1 si el clic era del selector
Items_AlClicTextDraw(playerid, PlayerText:td)
{
    if (!gSelAbierto[playerid]) return 0;

    if (td == gSelCerrar[playerid])
    {
        CerrarSelector(playerid);
        return 1;
    }

    new paginas = TotalPaginas(playerid);
    if (td == gSelAnterior[playerid])
    {
        gSelPagina[playerid] = (gSelPagina[playerid] + paginas - 1) % paginas;
        MostrarPagina(playerid);
        return 1;
    }
    if (td == gSelSiguiente[playerid])
    {
        gSelPagina[playerid] = (gSelPagina[playerid] + 1) % paginas;
        MostrarPagina(playerid);
        return 1;
    }

    for (new i = 0; i < SEL_POR_PAGINA; i++)
    {
        if (td != gSelModelos[playerid][i]) continue;

        new E_BANDO:bando = gJugador[playerid][jBando];
        new indice = gSelPagina[playerid] * SEL_POR_PAGINA + i;
        if (indice < Bando_TotalSkins(bando)) ElegirSkin(playerid, Bando_SkinEnIndice(bando, indice));
        return 1;
    }
    return 0;
}

static ElegirSkin(playerid, skin)
{
    CerrarSelector(playerid);

    gJugador[playerid][jSkin] = skin;
    // Si es humano, es el skin al que vuelve si lo curan
    if (gJugador[playerid][jBando] == BANDO_HUMANO) gJugador[playerid][jSkinSeleccion] = skin;
    SetPlayerSkin(playerid, skin);

    new mensaje[48];
    format(mensaje, sizeof(mensaje), "Cambiaste tu skin al %d.", skin);
    SendClientMessage(playerid, COLOR_EXITO, mensaje);
}

// ESC durante la seleccion: el cliente ya la cancelo
Items_AlCancelarSeleccion(playerid)
{
    CerrarSelector(playerid, false);
}

// Al morir o salir se cierra el selector (el bando puede cambiar)
Items_Cerrar(playerid)
{
    CerrarSelector(playerid);
}
