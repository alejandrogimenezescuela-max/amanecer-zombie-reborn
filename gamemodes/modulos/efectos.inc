// ------------------------------------------------------------
//  Efectos visuales
// ------------------------------------------------------------

forward BorrarEfecto(objeto);

// Particula en el lugar donde murio el jugador (humano o zombie), visible para todos
Efectos_AlMorir(playerid)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    // GetPlayerPos da el centro del cuerpo: ahi aparece la particula
    new objeto = CreateObject(MODELO_EFECTO_MUERTE, x, y, z, 0.0, 0.0, 0.0);
    if (objeto != INVALID_OBJECT_ID)
    {
        SetTimerEx("BorrarEfecto", DURACION_EFECTO_MUERTE, false, "d", objeto);
    }
}

public BorrarEfecto(objeto)
{
    DestroyObject(objeto);
    return 1;
}

// /luz: oculta o vuelve a mostrar los efectos de luz propios (manos de fuego del
// CEO y efecto del Nemesis). Cualquiera lo puede usar; si no tiene efectos, no cambia nada.
Efectos_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/luz", true)) return 0;

    gJugador[playerid][jSinLuz] = !gJugador[playerid][jSinLuz];
    Nemesis_ActualizarLuz(playerid);    // el fuego de las manos se actualiza solo en OnPlayerUpdate
    SendClientMessage(playerid, COLOR_INFO, gJugador[playerid][jSinLuz]
        ? ("Efectos de luz ocultos. Usa /luz para volver a mostrarlos.")
        : ("Efectos de luz visibles."));
    return 1;
}
