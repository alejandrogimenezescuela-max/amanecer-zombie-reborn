// ------------------------------------------------------------
//  /fly: modo vuelo para Administrador y CEO
//  W/S/A/D mueven hacia donde mira la camara, Espacio sube,
//  C baja, Shift acelera. Sin teclas se queda flotando.
// ------------------------------------------------------------

#define RANGO_MINIMO_VUELO      RANGO_ADMINISTRADOR

const Float:VUELO_VELOCIDAD = 0.6;          // ~30 m/s
const Float:VUELO_VELOCIDAD_RAPIDA = 2.0;   // con Shift

// Vida infinita mientras vuela: el juego calcula dano por caida al cambiar
// la velocidad de golpe (subir, frenar) y si no, el jugador muere en el aire
#define VIDA_INFINITA           (Float:0x7F800000)
#define DEMORA_RESTAURAR_VIDA   1000    // ms despues de aterrizar

forward RestaurarVidaVuelo(playerid, contador, Float:vida);

static bool:gVolando[MAX_PLAYERS];
static bool:gVueloEnMovimiento[MAX_PLAYERS];
static Float:gVidaAntesDeVolar[MAX_PLAYERS];

// Devuelve 1 si el comando era de este modulo
Vuelo_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/fly", true)) return 0;

    if (Admin_Rango(playerid) < RANGO_MINIMO_VUELO)
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }
    if (!gJugador[playerid][jEnJuego] || GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo podes volar a pie.");
        return 1;
    }

    if (gVolando[playerid])
    {
        Vuelo_Desactivar(playerid, true);
        SendClientMessage(playerid, COLOR_INFO, "Modo vuelo desactivado.");
    }
    else
    {
        gVolando[playerid] = true;
        gVueloEnMovimiento[playerid] = false;
        GetPlayerHealth(playerid, gVidaAntesDeVolar[playerid]);
        SetPlayerHealth(playerid, VIDA_INFINITA);
        AnimarVuelo(playerid, false);
        SendClientMessage(playerid, COLOR_EXITO, "Modo vuelo activado. W/S/A/D para moverte, Espacio sube, C baja, Shift acelera.");
    }
    return 1;
}

// 'aterrizar' baja al jugador al suelo y le devuelve la vida un instante despues
// (para que el aterrizaje no le haga dano). Al morir o salir no se aterriza ni se
// devuelve la vida con demora: se deja la vida guardada para no grabar "infinito".
Vuelo_Desactivar(playerid, bool:aterrizar = false)
{
    if (!gVolando[playerid]) return;
    gVolando[playerid] = false;

    SetPlayerVelocity(playerid, 0.0, 0.0, 0.0);
    ClearAnimations(playerid, SYNC_ALL);
    if (aterrizar)
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        SetPlayerPosFindZ(playerid, x, y, z);
        SetTimerEx("RestaurarVidaVuelo", DEMORA_RESTAURAR_VIDA, false, "ddf",
            playerid, gContadorConsulta[playerid], gVidaAntesDeVolar[playerid]);
    }
    else if (GetPlayerState(playerid) != PLAYER_STATE_WASTED)
    {
        SetPlayerHealth(playerid, gVidaAntesDeVolar[playerid]);
    }
}

public RestaurarVidaVuelo(playerid, contador, Float:vida)
{
    // Si volvio a volar mientras tanto, sigue invulnerable
    if (contador != gContadorConsulta[playerid] || gVolando[playerid]) return 1;
    SetPlayerHealth(playerid, vida);
    return 1;
}

// Se llama en cada OnPlayerUpdate: mueve al jugador segun la camara y las teclas
Vuelo_AlActualizar(playerid)
{
    if (!gVolando[playerid]) return;
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        Vuelo_Desactivar(playerid);
        return;
    }

    new KEY:teclas, adelante, costado;
    GetPlayerKeys(playerid, teclas, adelante, costado);

    new Float:fx, Float:fy, Float:fz;
    GetPlayerCameraFrontVector(playerid, fx, fy, fz);

    new Float:velocidad = (teclas & KEY_SPRINT) ? VUELO_VELOCIDAD_RAPIDA : VUELO_VELOCIDAD;
    new Float:vx = 0.0, Float:vy = 0.0, Float:vz = 0.0;

    // Adelante / atras: en la direccion exacta de la camara (incluye subir o bajar)
    if (adelante < 0)      { vx += fx; vy += fy; vz += fz; }
    else if (adelante > 0) { vx -= fx; vy -= fy; vz -= fz; }

    // Izquierda / derecha: perpendicular a la camara, sobre el plano
    if (costado < 0)      { vx -= fy; vy += fx; }
    else if (costado > 0) { vx += fy; vy -= fx; }

    if (teclas & KEY_JUMP)   vz += 1.0;
    if (teclas & KEY_CROUCH) vz -= 1.0;

    new bool:moviendose = (vx != 0.0 || vy != 0.0 || vz != 0.0);
    if (moviendose)
    {
        // Normaliza para que en diagonal no vaya mas rapido
        new Float:largo = floatsqroot(vx * vx + vy * vy + vz * vz);
        vx = vx / largo * velocidad;
        vy = vy / largo * velocidad;
        vz = vz / largo * velocidad;
        SetPlayerFacingAngle(playerid, atan2(fy, fx) - 90.0);
    }
    // Velocidad 0 en cada actualizacion = flotar en el lugar (anula la gravedad)
    SetPlayerVelocity(playerid, vx, vy, vz);

    if (moviendose != gVueloEnMovimiento[playerid])
    {
        gVueloEnMovimiento[playerid] = moviendose;
        AnimarVuelo(playerid, moviendose);
    }
}

// Pose de paracaidista: en caida libre al moverse, flotando al estar quieto
static AnimarVuelo(playerid, bool:moviendose)
{
    if (moviendose) ApplyAnimation(playerid, "PARACHUTE", "FALL_SkyDive", 4.1, true, false, false, false, 0, SYNC_ALL);
    else ApplyAnimation(playerid, "PARACHUTE", "PARA_float", 4.1, true, false, false, false, 0, SYNC_ALL);
}

// Precarga la libreria de animaciones (la primera ApplyAnimation de una libreria puede fallar)
Vuelo_PrecargarAnimaciones(playerid)
{
    ApplyAnimation(playerid, "PARACHUTE", "null", 0.0, false, false, false, false, 0);
}
