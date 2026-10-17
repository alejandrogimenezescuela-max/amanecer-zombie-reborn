// ------------------------------------------------------------
//  Datos de jugador
// ------------------------------------------------------------

enum E_BANDO
{
    BANDO_HUMANO,
    BANDO_ZOMBIE
}

enum E_JUGADOR
{
    jId,
    jHash[LARGO_HASH],
    jSalt[LARGO_SALT + 1],
    bool:jLogueado,
    bool:jEnJuego,          // ya spawneo despues de elegir skin
    jIntentos,
    jSkin,
    E_BANDO:jBando,
    jSkinSeleccion,         // skin que esta mirando en el selector
    Float:jVida,
    jDinero,                // el valor real; el del GTA es solo para mostrar
    jAdminNivel,            // rango guardado (RANGO_*); ver Admin_Rango
    bool:jMuerto,
    PlayerText:jTextoBando
}

new gJugador[MAX_PLAYERS][E_JUGADOR];

// Se incrementa al conectar y desconectar; si una respuesta de MySQL o un timer
// llega con un valor viejo, el jugador original ya se fue y se descarta.
new gContadorConsulta[MAX_PLAYERS];

// El CEO es el unico con acceso a los comandos y efectos de admin
EsCEO(playerid)
{
    return gJugador[playerid][jLogueado] && gJugador[playerid][jAdminNivel] == RANGO_CEO;
}

ReiniciarDatos(playerid)
{
    gJugador[playerid][jId] = 0;
    gJugador[playerid][jHash][0] = EOS;
    gJugador[playerid][jSalt][0] = EOS;
    gJugador[playerid][jLogueado] = false;
    gJugador[playerid][jEnJuego] = false;
    gJugador[playerid][jIntentos] = 0;
    gJugador[playerid][jSkin] = 0;
    gJugador[playerid][jBando] = BANDO_HUMANO;
    gJugador[playerid][jSkinSeleccion] = 0;
    gJugador[playerid][jVida] = 100.0;
    gJugador[playerid][jDinero] = 0;
    gJugador[playerid][jAdminNivel] = RANGO_NINGUNO;
    gJugador[playerid][jMuerto] = false;
    gJugador[playerid][jTextoBando] = INVALID_PLAYER_TEXT_DRAW;
}
