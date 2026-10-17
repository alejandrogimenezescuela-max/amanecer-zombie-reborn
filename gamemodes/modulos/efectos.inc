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
