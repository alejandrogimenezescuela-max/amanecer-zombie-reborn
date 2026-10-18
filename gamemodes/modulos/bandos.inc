// ------------------------------------------------------------
//  Bandos y selector de skin
// ------------------------------------------------------------

#define MAX_SKIN                311
#define SKIN_INVALIDO           74      // no existe en SA

// Skins exclusivos del bando zombie; el resto son de humano
static const gSkinsZombie[] = { 162, 230, 200, 239, 135, 134, 79, 75, 77 };

// Skin de cada clase, para saber cual esta mirando en el selector
static gSkinDeClase[MAX_SKIN + 1];

// Skins que puede usar un humano (se arma al crear las clases)
static gSkinsHumano[MAX_SKIN + 1];
static gTotalSkinsHumano;

// Lugar del selector: lobby de Zombotech (San Fierro), frente al cartel
const Float:SELECTOR_X = -1943.8931;
const Float:SELECTOR_Y = 708.0086;
const Float:SELECTOR_Z = 46.5625;
const Float:SELECTOR_ANGULO = 2.3837;

// Camara ~4 m delante del personaje (hacia donde mira), un poco mas alta que la cabeza
const Float:CAMARA_SELECTOR_X = -1944.0595;
const Float:CAMARA_SELECTOR_Y = 712.0051;
const Float:CAMARA_SELECTOR_Z = 47.2;

E_BANDO:Bando_DeSkin(skin)
{
    for (new i = 0; i < sizeof(gSkinsZombie); i++)
    {
        if (gSkinsZombie[i] == skin) return BANDO_ZOMBIE;
    }
    return BANDO_HUMANO;
}

Bando_Color(E_BANDO:bando)
{
    return (bando == BANDO_ZOMBIE) ? COLOR_ZOMBIE : COLOR_HUMANO;
}

Bando_Nombre(E_BANDO:bando, nombre[], largo = sizeof(nombre))
{
    format(nombre, largo, (bando == BANDO_ZOMBIE) ? ("ZOMBIE") : ("HUMANO"));
}

// Texto de la base de datos (columna users.bando)
Bando_Texto(E_BANDO:bando, texto[], largo = sizeof(texto))
{
    format(texto, largo, (bando == BANDO_ZOMBIE) ? ("zombie") : ("humano"));
}

// Una clase por cada skin valido, todas en Grove Street. Orden del selector:
// un humano, los zombies seguidos y despues el resto de los humanos. Asi la
// flecha derecha recorre los zombies y la izquierda da la vuelta a los humanos.
// El skin 0 (CJ) no se puede elegir.
Bando_CrearClases()
{
    AgregarClase(1);
    gSkinsHumano[gTotalSkinsHumano++] = 1;

    for (new i = 0; i < sizeof(gSkinsZombie); i++)
    {
        AgregarClase(gSkinsZombie[i]);
    }

    for (new skin = 2; skin <= MAX_SKIN; skin++)
    {
        if (skin == SKIN_INVALIDO || Bando_DeSkin(skin) == BANDO_ZOMBIE) continue;
        AgregarClase(skin);
        gSkinsHumano[gTotalSkinsHumano++] = skin;
    }
}

// Cantidad de skins disponibles para un bando (sin CJ)
Bando_TotalSkins(E_BANDO:bando)
{
    return (bando == BANDO_ZOMBIE) ? sizeof(gSkinsZombie) : gTotalSkinsHumano;
}

// Skin numero 'indice' de la lista del bando
Bando_SkinEnIndice(E_BANDO:bando, indice)
{
    return (bando == BANDO_ZOMBIE) ? gSkinsZombie[indice] : gSkinsHumano[indice];
}

static AgregarClase(skin)
{
    // Las clases estan en el lugar del selector; al spawnear se va a un spawn de su bando
    new clase = AddPlayerClass(skin, SELECTOR_X, SELECTOR_Y, SELECTOR_Z, SELECTOR_ANGULO);
    if (0 <= clase < sizeof(gSkinDeClase)) gSkinDeClase[clase] = skin;
}

// Texto "HUMANO" / "ZOMBIE" arriba de la pantalla
Bando_CrearTexto(playerid)
{
    new PlayerText:texto = CreatePlayerTextDraw(playerid, 320.0, 40.0, "_");
    PlayerTextDrawAlignment(playerid, texto, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(playerid, texto, TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, texto, 0.6, 2.4);
    PlayerTextDrawSetOutline(playerid, texto, 1);
    gJugador[playerid][jTextoBando] = texto;
}

Bando_OcultarTexto(playerid)
{
    if (gJugador[playerid][jTextoBando] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawHide(playerid, gJugador[playerid][jTextoBando]);
    }
}

// Se llama en OnPlayerRequestClass con el jugador ya logueado
Bando_MostrarSkin(playerid, classid)
{
    new skin = (0 <= classid < sizeof(gSkinDeClase)) ? gSkinDeClase[classid] : 0;
    new E_BANDO:bando = Bando_DeSkin(skin);
    gJugador[playerid][jSkinSeleccion] = skin;

    SetPlayerInterior(playerid, 0);
    SetPlayerPos(playerid, SELECTOR_X, SELECTOR_Y, SELECTOR_Z);
    SetPlayerFacingAngle(playerid, SELECTOR_ANGULO);
    SetPlayerCameraPos(playerid, CAMARA_SELECTOR_X, CAMARA_SELECTOR_Y, CAMARA_SELECTOR_Z);
    SetPlayerCameraLookAt(playerid, SELECTOR_X, SELECTOR_Y, SELECTOR_Z + 0.3);

    new nombre[8];
    Bando_Nombre(bando, nombre);
    PlayerTextDrawSetString(playerid, gJugador[playerid][jTextoBando], nombre);
    PlayerTextDrawColour(playerid, gJugador[playerid][jTextoBando], Bando_Color(bando));
    PlayerTextDrawShow(playerid, gJugador[playerid][jTextoBando]);
}

// Musica de fondo desde el login hasta que spawnea (se llama una vez)
Bando_IniciarMusica(playerid)
{
    if (Ajustes_Activado(playerid, CFG_SIN_MUSICA)) PlayAudioStreamForPlayer(playerid, MUSICA_SELECTOR);
}

// Se llama en OnPlayerRequestSpawn: fija el skin y el bando elegidos
Bando_ConfirmarSeleccion(playerid)
{
    StopAudioStreamForPlayer(playerid);
    gJugador[playerid][jSkin] = gJugador[playerid][jSkinSeleccion];
    gJugador[playerid][jBando] = Bando_DeSkin(gJugador[playerid][jSkin]);
    Bando_OcultarTexto(playerid);
}

// Si ya estaba jugando (murio y el juego reabrio la seleccion), no lo deja
// elegir de nuevo: spawnea directo con su skin y bando actuales
Bando_SaltearSelector(playerid)
{
    Bando_OcultarTexto(playerid);
    SetSpawnInfo(playerid, NO_TEAM, gJugador[playerid][jSkin],
        SPAWN_INICIAL_X, SPAWN_INICIAL_Y, SPAWN_INICIAL_Z, SPAWN_INICIAL_ANGULO);
    SpawnPlayer(playerid);
}

// Un humano que muere se convierte en zombie; un zombie sigue igual
Bando_AlMorir(playerid)
{
    if (gJugador[playerid][jBando] == BANDO_HUMANO) Convertir(playerid);

    // La posicion la decide el spawn segun el bando; aca solo importa el skin
    SetSpawnInfo(playerid, NO_TEAM, gJugador[playerid][jSkin],
        SPAWN_INICIAL_X, SPAWN_INICIAL_Y, SPAWN_INICIAL_Z, SPAWN_INICIAL_ANGULO);
}

// Infecta a un humano vivo en el momento (comando /virus)
Bando_Infectar(playerid)
{
    Convertir(playerid);
    AplicarBando(playerid);
}

// Pasa a zombie con un skin de zombie al azar y avisa a todos
static Convertir(playerid)
{
    gJugador[playerid][jBando] = BANDO_ZOMBIE;
    gJugador[playerid][jSkin] = gSkinsZombie[random(sizeof(gSkinsZombie))];
    AvisarInfeccion(playerid);
}

// Aviso en rojo a todo el servidor (incluido el infectado) y su mensaje propio
static AvisarInfeccion(playerid)
{
    new nombre[MAX_PLAYER_NAME + 1], mensaje[64];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(mensaje, sizeof(mensaje), "[<!>] %s ha sido infectado.", nombre);
    AvisarATodos(COLOR_ERROR, mensaje);
    SendClientMessage(playerid, COLOR_INFECTADO, "Ahora estas infectado.");
}

// Mensaje a todos los jugadores que ya iniciaron sesion
static AvisarATodos(color, const mensaje[])
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && gJugador[i][jLogueado])
        {
            SendClientMessage(i, color, mensaje);
        }
    }
}

// Vuelve a ser humano: el skin que eligio en el selector si era de humano,
// si no uno de humano al azar. Recibe el kit y vida llena.
Bando_Curar(playerid)
{
    new skin = gJugador[playerid][jSkinSeleccion];
    while (skin == 0 || skin == SKIN_INVALIDO || Bando_DeSkin(skin) == BANDO_ZOMBIE)
    {
        skin = 1 + random(MAX_SKIN);
    }

    gJugador[playerid][jBando] = BANDO_HUMANO;
    gJugador[playerid][jSkin] = skin;
    SetPlayerHealth(playerid, 100.0);

    new nombre[MAX_PLAYER_NAME + 1], mensaje[64];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(mensaje, sizeof(mensaje), "[<!>] %s ha sido curado.", nombre);
    AvisarATodos(COLOR_HUMANO, mensaje);
    SendClientMessage(playerid, COLOR_HUMANO, "Te curaron. Vuelves a ser humano.");

    AplicarBando(playerid);
}

Bando_AlSpawnear(playerid)
{
    Bando_OcultarTexto(playerid);
    AplicarBando(playerid);
}

// Skin, color del nombre y del radar segun el bando. Los humanos reciben el
// kit de armas y dicen una frase; los zombies van sin armas.
static AplicarBando(playerid)
{
    new E_BANDO:bando = gJugador[playerid][jBando];
    SetPlayerSkin(playerid, gJugador[playerid][jSkin]);
    SetPlayerColor(playerid, Bando_Color(bando));
    ResetPlayerWeapons(playerid);

    if (bando == BANDO_HUMANO)
    {
        GivePlayerWeapon(playerid, WEAPON_AK47, MUNICION_AK47);
        GivePlayerWeapon(playerid, WEAPON_MP5, MUNICION_MP5);
        GivePlayerWeapon(playerid, WEAPON_DEAGLE, MUNICION_DEAGLE);

        SendClientMessage(playerid, COLOR_HUMANO, "Eres un humano, intenta sobrevivir a la infeccion.");
        DecirFraseHumano(playerid);
    }
    else
    {
        SendClientMessage(playerid, COLOR_INFECTADO, "Eres un zombie, busca otros humanos para infectar.");
    }
}

// Chat: el nombre en verde si es humano o en blanco si es zombie
Bando_AlHablar(playerid, const texto[])
{
    new nombre[MAX_PLAYER_NAME + 1], mensaje[144], limpio[144];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    strcat(limpio, texto);
    LimpiarColores(limpio);

    format(mensaje, sizeof(mensaje), "%s%s: {DDDDDD}%s",
        (gJugador[playerid][jBando] == BANDO_ZOMBIE) ? (EMBED_ZOMBIE) : (EMBED_HUMANO), nombre, limpio);
    SendClientMessageToAll(COLOR_INFO, mensaje);
}

static const gFrasesHumano[][] =
{
    "Que noche larga...",
    "Buscare mas sobrevivientes.",
    "Tengo que encontrar municion.",
    "No puedo bajar la guardia.",
    "Alguien mas sigue con vida?",
    "Ojala que amanezca pronto.",
    "Tengo que mantenerme en movimiento.",
    "Esas cosas estan por todas partes.",
    "No pienso terminar como ellos.",
    "Necesito encontrar un lugar seguro."
};

// Burbuja sobre la cabeza (la ven los demas) y mensaje en el chat propio
static DecirFraseHumano(playerid)
{
    new frase = random(sizeof(gFrasesHumano)), nombre[MAX_PLAYER_NAME + 1], mensaje[96];
    SetPlayerChatBubble(playerid, gFrasesHumano[frase], COLOR_HUMANO, 30.0, 6000);

    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(mensaje, sizeof(mensaje), EMBED_HUMANO "%s: {DDDDDD}%s", nombre, gFrasesHumano[frase]);
    SendClientMessage(playerid, COLOR_INFO, mensaje);
}
