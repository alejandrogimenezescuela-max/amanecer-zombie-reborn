// ------------------------------------------------------------
//  Skatepark de Los Santos: la resistencia humana tomo una instalacion
//  de Umbrella enfrente. Mensajes de radio periodicos, /park para ir y
//  el checkpoint de la cura: un zombie que entra queda quieto unos
//  segundos y vuelve a ser humano.
// ------------------------------------------------------------

#define RADIO_INTERVALO         (5 * 60 * 1000)     // cada 5 minutos

forward TransmitirRadio();
forward TerminarCura(playerid, contador);

// Transmision militar: encabezado en rojo y dos renglones en blanco
#define COLOR_RADIO_TITULO      0xFF3333FF
#define COLOR_RADIO_TEXTO       0xFFFFFFFF
static const gRadioTitulo[] = "|---------| Transmision Militar |---------|";
static const gRadioLineas[][] =
{
    "Hemos tomado la base Umbrella en Los Santos y tenemos la cura al virus.",
    "El campamento se visualiza en cualquier radar, sur de Glen Park."
};

// Icono de la cruz roja en el radar (el de hospital del GTA), visible en todo el mapa
#define ICONO_CURA_SLOT         0       // indice de icono del jugador (0-99)
#define ICONO_CURA_MODELO       22      // cruz roja

// Checkpoint de la cura: frente al skatepark, en la instalacion de Umbrella.
// La Z es la del suelo (la posicion guardada con /save es el centro del cuerpo, ~1 m mas alto).
const Float:CURA_X = 1739.8788;
const Float:CURA_Y = -1424.3577;
const Float:CURA_Z = 33.8133;
const Float:CURA_RADIO = 2.0;
#define CURA_DURACION           5000    // ms quieto mientras se aplica el antidoto

static bool:gCurando[MAX_PLAYERS];

// Lugares de llegada de /park alrededor del skatepark (x, y, z, angulo).
// Altura fija a nivel de calle: SetPlayerPosFindZ falla si la zona no esta
// cargada (el jugador viene de lejos) y lo deja caer al vacio.
static const Float:gLlegadasParque[][4] =
{
    { 1900.0, -1390.0, 13.6, 225.0 },
    { 1940.0, -1395.0, 13.6, 135.0 },
    { 1915.0, -1430.0, 13.6,   0.0 }
};
#define PARQUE_CONGELAR_MS      1000    // quieto al llegar, mientras carga el mapa

forward DescongelarParque(playerid, contador);

Parque_Iniciar()
{
    SetTimer("TransmitirRadio", RADIO_INTERVALO, true);
}

public TransmitirRadio()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i) || !gJugador[i][jLogueado] || !Ajustes_Activado(i, CFG_SIN_RADIO)) continue;

        SendClientMessage(i, COLOR_RADIO_TITULO, gRadioTitulo);
        for (new l = 0; l < sizeof(gRadioLineas); l++)
        {
            SendClientMessage(i, COLOR_RADIO_TEXTO, gRadioLineas[l]);
        }
    }
    return 1;
}

// Devuelve 1 si el comando era de este modulo
Parque_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/park", true)) return 0;

    if (!gJugador[playerid][jEnJuego] || GetPlayerState(playerid) == PLAYER_STATE_WASTED)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ahora no podes usar este comando.");
        return 1;
    }

    new i = random(sizeof(gLlegadasParque));
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    TogglePlayerControllable(playerid, false);
    SetPlayerPos(playerid, gLlegadasParque[i][0], gLlegadasParque[i][1], gLlegadasParque[i][2]);
    SetPlayerFacingAngle(playerid, gLlegadasParque[i][3]);
    SetCameraBehindPlayer(playerid);
    SetTimerEx("DescongelarParque", PARQUE_CONGELAR_MS, false, "dd", playerid, gContadorConsulta[playerid]);
    return 1;
}

public DescongelarParque(playerid, contador)
{
    if (contador == gContadorConsulta[playerid]) TogglePlayerControllable(playerid, true);
    return 1;
}

// ------------------------------------------------------------
//  Checkpoint de la cura
// ------------------------------------------------------------

// Al spawnear: marca el checkpoint y la cruz roja en el radar (todos lo ven; solo cura a zombies).
// Si murio mientras lo curaban, se descongela.
Parque_AlSpawnear(playerid)
{
    if (gCurando[playerid])
    {
        gCurando[playerid] = false;
        TogglePlayerControllable(playerid, true);
    }
    SetPlayerCheckpoint(playerid, CURA_X, CURA_Y, CURA_Z, CURA_RADIO);
    SetPlayerMapIcon(playerid, ICONO_CURA_SLOT, CURA_X, CURA_Y, CURA_Z, ICONO_CURA_MODELO, 0, MAPICON_GLOBAL);
}

Parque_AlSalir(playerid)
{
    gCurando[playerid] = false;
}

// Un zombie que entra queda quieto CURA_DURACION y vuelve a ser humano
Parque_AlEntrarCheckpoint(playerid)
{
    if (!gJugador[playerid][jEnJuego] || gCurando[playerid]) return;
    if (gJugador[playerid][jBando] != BANDO_ZOMBIE) return;
    if (Nemesis_Es(playerid))
    {
        GameTextForPlayer(playerid, "~r~EL ANTIDOTO NO FUNCIONA EN NEMESIS", 3000, 3);
        return;
    }

    gCurando[playerid] = true;
    TogglePlayerControllable(playerid, false);
    GameTextForPlayer(playerid, "~g~APLICANDO ANTIDOTO", CURA_DURACION, 3);
    SetTimerEx("TerminarCura", CURA_DURACION, false, "dd", playerid, gContadorConsulta[playerid]);
}

public TerminarCura(playerid, contador)
{
    if (contador != gContadorConsulta[playerid] || !gCurando[playerid]) return 1;
    gCurando[playerid] = false;
    TogglePlayerControllable(playerid, true);

    // Si murio o dejo de ser zombie en el medio (por ejemplo con /cure), no se aplica
    if (GetPlayerState(playerid) == PLAYER_STATE_WASTED || gJugador[playerid][jBando] != BANDO_ZOMBIE) return 1;

    // Sin los mensajes del spawn: no dice la frase de humano
    Bando_Curar(playerid, false);
    if (EsCEO(playerid)) Admin_AlSpawnear(playerid);
    return 1;
}
