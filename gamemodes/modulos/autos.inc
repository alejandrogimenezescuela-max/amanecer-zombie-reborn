// ------------------------------------------------------------
//  /autos: tienda de autos con la galeria de vistas previas.
//  Solo autos comunes (sin motos, militares, tanques ni aereos).
//  Cada jugador tiene un auto a la vez: comprar otro reemplaza al
//  anterior, y se borra al desconectarse o si explota.
// ------------------------------------------------------------

#define DEMORA_BORRAR_AUTO      5000    // ms despues de explotar

// Ordenados de mas barato a mas caro
static const gAutosModelo[] = { 410, 404, 436, 492, 466, 496, 439, 405, 567, 400, 579, 560, 480, 415, 411 };
static const gAutosPrecio[] =
{
     8000, 10000, 12000, 12000, 15000, 18000, 20000, 25000,
    25000, 30000, 45000, 65000, 70000, 100000, 120000
};
static const gAutosNombre[][] =
{
    "Manana", "Perenniel", "Previon", "Greenwood", "Glendale", "Blista Compact", "Stallion", "Sentinel",
    "Savanna", "Landstalker", "Huntley", "Sultan", "Comet", "Cheetah", "Infernus"
};

// Colores de cada auto (carroceria, detalles) con los ids de color del GTA:
// 0 negro, 1 blanco, 2 celeste, 3 rojo, 6 amarillo, 16 verde oscuro, 79 azul,
// 86 verde, 126 rosa. Se usan en la vista previa y en el auto que se compra.
static const gAutosColor[][2] =
{
    { 3, 1 },   { 79, 1 },  { 6, 0 },   { 16, 1 },  { 2, 1 },
    { 126, 1 }, { 0, 3 },   { 1, 79 },  { 86, 0 },  { 0, 0 },
    { 1, 1 },   { 79, 0 },  { 3, 0 },   { 6, 6 },   { 3, 3 }
};

static gAutoDe[MAX_PLAYERS];            // vehiculo de cada jugador (0 = ninguno)

forward BorrarAutoExplotado(vehiculo);

// --- Datos para la galeria ---

Autos_Total()
{
    return sizeof(gAutosModelo);
}

Autos_Modelo(indice)
{
    return gAutosModelo[indice];
}

Autos_Colores(indice, &color1, &color2)
{
    color1 = gAutosColor[indice][0];
    color2 = gAutosColor[indice][1];
}

// "Sultan~n~$65.000" (~n~ = salto de linea en un textdraw)
Autos_Etiqueta(indice, destino[], largo)
{
    new precio[20];
    FormatearDinero(gAutosPrecio[indice], precio);
    format(destino, largo, "%s~n~%s", gAutosNombre[indice], precio);
}

// --- Comando y compra ---

// Devuelve 1 si el comando era de este modulo
Autos_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/autos", true)) return 0;

    if (!gJugador[playerid][jEnJuego] || Galeria_EstaAbierta(playerid)) return 1;
    if (GetPlayerInterior(playerid) != 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "No podes comprar un auto en un interior.");
        return 1;
    }
    Galeria_Abrir(playerid, GALERIA_AUTOS);
    return 1;
}

// Elegido en la galeria
Autos_Comprar(playerid, indice)
{
    if (indice < 0 || indice >= sizeof(gAutosModelo) || !gJugador[playerid][jEnJuego]) return;

    new item[40];
    format(item, sizeof(item), "un %s", gAutosNombre[indice]);
    if (!Dinero_Cobrar(playerid, gAutosPrecio[indice], item)) return;

    // Un auto por jugador: el anterior desaparece
    if (gAutoDe[playerid]) DestroyVehicle(gAutoDe[playerid]);

    new Float:x, Float:y, Float:z, Float:angulo;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angulo);

    // Los mismos colores de la vista previa; sin reaparecer (-1): es del jugador, no del mapa
    new vehiculo = CreateVehicle(gAutosModelo[indice], x, y, z + 0.5, angulo,
        gAutosColor[indice][0], gAutosColor[indice][1], -1);
    if (vehiculo == INVALID_VEHICLE_ID)
    {
        gAutoDe[playerid] = 0;
        Dinero_Dar(playerid, gAutosPrecio[indice]);
        SendClientMessage(playerid, COLOR_ERROR, "No se pudo crear el auto. Te devolvimos el dinero.");
        return;
    }
    SetVehicleVirtualWorld(vehiculo, GetPlayerVirtualWorld(playerid));
    gAutoDe[playerid] = vehiculo;
    PutPlayerInVehicle(playerid, vehiculo, 0);
}

// --- Limpieza ---

Autos_AlSalir(playerid)
{
    if (gAutoDe[playerid]) DestroyVehicle(gAutoDe[playerid]);
    gAutoDe[playerid] = 0;
}

// Un auto comprado que explota se borra un rato despues (no reaparece)
Autos_AlMorirVehiculo(vehiculo)
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (gAutoDe[i] != vehiculo) continue;
        gAutoDe[i] = 0;
        SetTimerEx("BorrarAutoExplotado", DEMORA_BORRAR_AUTO, false, "d", vehiculo);
        return;
    }
}

public BorrarAutoExplotado(vehiculo)
{
    // Si en el medio alguien compro otro auto con el mismo id, no se toca
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (gAutoDe[i] == vehiculo) return 1;
    }
    DestroyVehicle(vehiculo);
    return 1;
}
