// ------------------------------------------------------------
//  Skatepark de Los Santos: base de la resistencia en la historia.
//  Mensajes de radio periodicos y /park para ir hasta ahi.
// ------------------------------------------------------------

#define RADIO_INTERVALO         (5 * 60 * 1000)     // cada 5 minutos

forward TransmitirRadio();

// Se envian en orden, uno por intervalo, y vuelven a empezar
static const gMensajesRadio[][] =
{
    // Maximo ~105 caracteres: el prefijo con colores ocupa 35 y el chat corta en 144
    "Aqui la resistencia. Tomamos una instalacion de Umbrella en el skatepark de Los Santos. Usa /park.",
    "Sobrevivientes: el skatepark es nuestro punto de reunion. No se queden solos ahi afuera. Usa /park.",
    "Encontramos muestras del virus en el laboratorio de Umbrella. Trabajamos en una cura en el skatepark.",
    "Si alguien escucha esto: vengan al skatepark (/park) antes de que caiga la noche. No vayan solos."
};

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

static gProximoMensajeRadio;

Parque_Iniciar()
{
    SetTimer("TransmitirRadio", RADIO_INTERVALO, true);
}

public TransmitirRadio()
{
    new mensaje[144];
    format(mensaje, sizeof(mensaje), "{FFA500}[Radio transmisor] {E0E0E0}%s", gMensajesRadio[gProximoMensajeRadio]);
    gProximoMensajeRadio = (gProximoMensajeRadio + 1) % sizeof(gMensajesRadio);

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && gJugador[i][jLogueado] && Ajustes_Activado(i, CFG_SIN_RADIO))
        {
            SendClientMessage(i, COLOR_INFO, mensaje);
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
