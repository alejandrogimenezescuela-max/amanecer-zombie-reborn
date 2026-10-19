// ------------------------------------------------------------
//  Galeria: grilla de vistas previas 3D clickeables (textdraws).
//  La usan el selector de skin de /item y la tienda de /autos.
//  Cada tipo de galeria define sus elementos en las funciones de
//  "Datos de cada tipo" (cantidad, modelo, etiqueta y que hacer al elegir).
// ------------------------------------------------------------

#define GALERIA_NINGUNA         0
#define GALERIA_SKINS           1
#define GALERIA_AUTOS           2

// Grilla de 5 x 3 vistas previas por pagina
#define GAL_COLUMNAS            5
#define GAL_FILAS               3
#define GAL_POR_PAGINA          (GAL_COLUMNAS * GAL_FILAS)

const Float:GAL_X = 125.0;              // esquina del fondo
const Float:GAL_Y = 110.0;
const Float:GAL_ANCHO = 390.0;
const Float:GAL_ALTO = 275.0;
const Float:GAL_CASILLA = 75.0;         // separacion entre vistas previas
const Float:GAL_MODELO = 70.0;          // tamano de cada vista previa

#define GAL_COLOR_FONDO         0x000000CC
#define GAL_COLOR_CASILLA       0x2A2A2AFF
#define GAL_COLOR_BOTON         0x444444FF
#define GAL_COLOR_HOVER         0x00CC00FF

static gGalTipo[MAX_PLAYERS];           // GALERIA_NINGUNA = cerrada
static gGalPagina[MAX_PLAYERS];
static PlayerText:gGalFondo[MAX_PLAYERS];
static PlayerText:gGalTitulo[MAX_PLAYERS];
static PlayerText:gGalModelos[MAX_PLAYERS][GAL_POR_PAGINA];
static PlayerText:gGalEtiquetas[MAX_PLAYERS][GAL_POR_PAGINA];
static PlayerText:gGalAnterior[MAX_PLAYERS];
static PlayerText:gGalSiguiente[MAX_PLAYERS];
static PlayerText:gGalCerrar[MAX_PLAYERS];

// ------------------------------------------------------------
//  Datos de cada tipo
// ------------------------------------------------------------

static TotalElementos(playerid)
{
    switch (gGalTipo[playerid])
    {
        case GALERIA_SKINS: return Bando_TotalSkins(gJugador[playerid][jBando]);
        case GALERIA_AUTOS: return Autos_Total();
    }
    return 0;
}

static ModeloDe(playerid, indice)
{
    switch (gGalTipo[playerid])
    {
        case GALERIA_SKINS: return Bando_SkinEnIndice(gJugador[playerid][jBando], indice);
        case GALERIA_AUTOS: return Autos_Modelo(indice);
    }
    return 0;
}

// Texto debajo de la vista previa (vacio = sin etiqueta)
static EtiquetaDe(playerid, indice, destino[], largo = sizeof(destino))
{
    destino[0] = EOS;
    if (gGalTipo[playerid] == GALERIA_AUTOS) Autos_Etiqueta(indice, destino, largo);
}

static TituloDe(playerid, destino[], largo = sizeof(destino))
{
    if (gGalTipo[playerid] == GALERIA_SKINS)
    {
        new nombre[8];
        Bando_Nombre(gJugador[playerid][jBando], nombre);
        format(destino, largo, "Skins de %s", nombre);
    }
    else
    {
        format(destino, largo, "Autos");
    }
}

static ColorTitulo(playerid)
{
    return (gGalTipo[playerid] == GALERIA_SKINS) ? Bando_Color(gJugador[playerid][jBando]) : 0xFFFFFFFF;
}

// La galeria ya se cerro cuando se llama
static ElegirElemento(playerid, tipo, indice)
{
    switch (tipo)
    {
        case GALERIA_SKINS: Items_ElegirSkin(playerid, Bando_SkinEnIndice(gJugador[playerid][jBando], indice));
        case GALERIA_AUTOS: Autos_Comprar(playerid, indice);
    }
}

// ------------------------------------------------------------
//  Textdraws
// ------------------------------------------------------------

Galeria_EstaAbierta(playerid)
{
    return gGalTipo[playerid] != GALERIA_NINGUNA;
}

static PlayerText:CrearBoton(playerid, Float:x, Float:y, const texto[], Float:ancho)
{
    new PlayerText:boton = CreatePlayerTextDraw(playerid, x, y, texto);
    PlayerTextDrawAlignment(playerid, boton, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(playerid, boton, TEXT_DRAW_FONT_AHARONI_BOLD);
    PlayerTextDrawLetterSize(playerid, boton, 0.3, 1.3);
    PlayerTextDrawSetOutline(playerid, boton, 1);
    PlayerTextDrawColour(playerid, boton, 0xFFFFFFFF);
    PlayerTextDrawUseBox(playerid, boton, true);
    PlayerTextDrawBoxColour(playerid, boton, GAL_COLOR_BOTON);
    // Con alineacion centrada, el segundo valor es el ancho de la caja (y del area clickeable)
    PlayerTextDrawTextSize(playerid, boton, 12.0, ancho);
    PlayerTextDrawSetSelectable(playerid, boton, true);
    return boton;
}

Galeria_Abrir(playerid, tipo)
{
    if (Galeria_EstaAbierta(playerid)) return;
    gGalTipo[playerid] = tipo;
    gGalPagina[playerid] = 0;

    // Fondo
    gGalFondo[playerid] = CreatePlayerTextDraw(playerid, GAL_X, GAL_Y, "LD_SPAC:white");
    PlayerTextDrawFont(playerid, gGalFondo[playerid], TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawTextSize(playerid, gGalFondo[playerid], GAL_ANCHO, GAL_ALTO);
    PlayerTextDrawColour(playerid, gGalFondo[playerid], GAL_COLOR_FONDO);

    // Titulo con la pagina
    gGalTitulo[playerid] = CreatePlayerTextDraw(playerid, GAL_X + GAL_ANCHO / 2.0, GAL_Y + 5.0, "_");
    PlayerTextDrawAlignment(playerid, gGalTitulo[playerid], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(playerid, gGalTitulo[playerid], TEXT_DRAW_FONT_AHARONI_BOLD);
    PlayerTextDrawLetterSize(playerid, gGalTitulo[playerid], 0.35, 1.5);
    PlayerTextDrawSetOutline(playerid, gGalTitulo[playerid], 1);
    PlayerTextDrawColour(playerid, gGalTitulo[playerid], ColorTitulo(playerid));

    // Vistas previas y sus etiquetas
    for (new i = 0; i < GAL_POR_PAGINA; i++)
    {
        new Float:x = GAL_X + 10.0 + float(i % GAL_COLUMNAS) * GAL_CASILLA;
        new Float:y = GAL_Y + 28.0 + float(i / GAL_COLUMNAS) * GAL_CASILLA;

        new PlayerText:modelo = CreatePlayerTextDraw(playerid, x, y, "_");
        PlayerTextDrawFont(playerid, modelo, TEXT_DRAW_FONT_MODEL_PREVIEW);
        PlayerTextDrawTextSize(playerid, modelo, GAL_MODELO, GAL_MODELO);
        PlayerTextDrawUseBox(playerid, modelo, true);
        PlayerTextDrawBoxColour(playerid, modelo, GAL_COLOR_CASILLA);
        PlayerTextDrawBackgroundColour(playerid, modelo, GAL_COLOR_CASILLA);
        if (tipo == GALERIA_AUTOS)
        {
            // Autos vistos de costado y un poco desde arriba (los colores van en MostrarPagina)
            PlayerTextDrawSetPreviewRot(playerid, modelo, -16.0, 0.0, -55.0, 1.0);
        }
        else
        {
            PlayerTextDrawSetPreviewRot(playerid, modelo, -10.0, 0.0, -20.0, 1.0);
        }
        PlayerTextDrawSetSelectable(playerid, modelo, true);
        gGalModelos[playerid][i] = modelo;

        // Nombre y precio sobre la parte de abajo de la casilla
        new PlayerText:etiqueta = CreatePlayerTextDraw(playerid, x + GAL_MODELO / 2.0, y + GAL_MODELO - 17.0, "_");
        PlayerTextDrawAlignment(playerid, etiqueta, TEXT_DRAW_ALIGN_CENTER);
        PlayerTextDrawFont(playerid, etiqueta, TEXT_DRAW_FONT_AHARONI_BOLD);
        PlayerTextDrawLetterSize(playerid, etiqueta, 0.18, 0.8);
        PlayerTextDrawSetOutline(playerid, etiqueta, 1);
        PlayerTextDrawColour(playerid, etiqueta, 0xFFFFFFFF);
        gGalEtiquetas[playerid][i] = etiqueta;
    }

    // Botones
    new Float:yBotones = GAL_Y + GAL_ALTO - 20.0;
    gGalAnterior[playerid] = CrearBoton(playerid, GAL_X + 50.0, yBotones, "<<", 50.0);
    gGalCerrar[playerid] = CrearBoton(playerid, GAL_X + GAL_ANCHO / 2.0, yBotones, "Cerrar", 70.0);
    gGalSiguiente[playerid] = CrearBoton(playerid, GAL_X + GAL_ANCHO - 50.0, yBotones, ">>", 50.0);

    PlayerTextDrawShow(playerid, gGalFondo[playerid]);
    PlayerTextDrawShow(playerid, gGalCerrar[playerid]);
    MostrarPagina(playerid);
    SelectTextDraw(playerid, GAL_COLOR_HOVER);
}

static TotalPaginas(playerid)
{
    return (TotalElementos(playerid) + GAL_POR_PAGINA - 1) / GAL_POR_PAGINA;
}

static MostrarPagina(playerid)
{
    new total = TotalElementos(playerid), paginas = TotalPaginas(playerid), etiqueta[48];

    for (new i = 0; i < GAL_POR_PAGINA; i++)
    {
        new indice = gGalPagina[playerid] * GAL_POR_PAGINA + i;
        if (indice < total)
        {
            PlayerTextDrawSetPreviewModel(playerid, gGalModelos[playerid][i], ModeloDe(playerid, indice));
            if (gGalTipo[playerid] == GALERIA_AUTOS)
            {
                new color1, color2;
                Autos_Colores(indice, color1, color2);
                PlayerTextDrawSetPreviewVehicleColours(playerid, gGalModelos[playerid][i], color1, color2);
            }
            PlayerTextDrawShow(playerid, gGalModelos[playerid][i]);

            EtiquetaDe(playerid, indice, etiqueta);
            if (etiqueta[0] != EOS)
            {
                PlayerTextDrawSetString(playerid, gGalEtiquetas[playerid][i], etiqueta);
                PlayerTextDrawShow(playerid, gGalEtiquetas[playerid][i]);
            }
            else PlayerTextDrawHide(playerid, gGalEtiquetas[playerid][i]);
        }
        else
        {
            PlayerTextDrawHide(playerid, gGalModelos[playerid][i]);
            PlayerTextDrawHide(playerid, gGalEtiquetas[playerid][i]);
        }
    }

    new titulo[48], texto[80];
    TituloDe(playerid, titulo);
    if (paginas > 1) format(texto, sizeof(texto), "%s  -  Pagina %d/%d", titulo, gGalPagina[playerid] + 1, paginas);
    else format(texto, sizeof(texto), "%s", titulo);
    PlayerTextDrawSetString(playerid, gGalTitulo[playerid], texto);
    PlayerTextDrawShow(playerid, gGalTitulo[playerid]);

    // Las flechas solo aparecen si hay mas de una pagina
    if (paginas > 1)
    {
        PlayerTextDrawShow(playerid, gGalAnterior[playerid]);
        PlayerTextDrawShow(playerid, gGalSiguiente[playerid]);
    }
    else
    {
        PlayerTextDrawHide(playerid, gGalAnterior[playerid]);
        PlayerTextDrawHide(playerid, gGalSiguiente[playerid]);
    }
}

// Cierra la galeria; 'cancelar' es false cuando el jugador ya salio con ESC
Galeria_Cerrar(playerid, bool:cancelar = true)
{
    if (!Galeria_EstaAbierta(playerid)) return;
    gGalTipo[playerid] = GALERIA_NINGUNA;

    PlayerTextDrawDestroy(playerid, gGalFondo[playerid]);
    PlayerTextDrawDestroy(playerid, gGalTitulo[playerid]);
    for (new i = 0; i < GAL_POR_PAGINA; i++)
    {
        PlayerTextDrawDestroy(playerid, gGalModelos[playerid][i]);
        PlayerTextDrawDestroy(playerid, gGalEtiquetas[playerid][i]);
    }
    PlayerTextDrawDestroy(playerid, gGalAnterior[playerid]);
    PlayerTextDrawDestroy(playerid, gGalSiguiente[playerid]);
    PlayerTextDrawDestroy(playerid, gGalCerrar[playerid]);

    if (cancelar) CancelSelectTextDraw(playerid);
}

// Devuelve 1 si el clic era de la galeria
Galeria_AlClicTextDraw(playerid, PlayerText:td)
{
    if (!Galeria_EstaAbierta(playerid)) return 0;

    if (td == gGalCerrar[playerid])
    {
        Galeria_Cerrar(playerid);
        return 1;
    }

    new paginas = TotalPaginas(playerid);
    if (td == gGalAnterior[playerid])
    {
        gGalPagina[playerid] = (gGalPagina[playerid] + paginas - 1) % paginas;
        MostrarPagina(playerid);
        return 1;
    }
    if (td == gGalSiguiente[playerid])
    {
        gGalPagina[playerid] = (gGalPagina[playerid] + 1) % paginas;
        MostrarPagina(playerid);
        return 1;
    }

    for (new i = 0; i < GAL_POR_PAGINA; i++)
    {
        if (td != gGalModelos[playerid][i]) continue;

        new indice = gGalPagina[playerid] * GAL_POR_PAGINA + i, tipo = gGalTipo[playerid];
        if (indice < TotalElementos(playerid))
        {
            Galeria_Cerrar(playerid);
            ElegirElemento(playerid, tipo, indice);
        }
        return 1;
    }
    return 0;
}

// ESC durante la seleccion: el cliente ya la cancelo
Galeria_AlCancelarSeleccion(playerid)
{
    Galeria_Cerrar(playerid, false);
}
