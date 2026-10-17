// ------------------------------------------------------------
//  Dinero: el valor real vive en el servidor (jDinero) y se
//  guarda en users.dinero; el contador del GTA solo lo muestra.
// ------------------------------------------------------------

#define DINERO_MAXIMO           999999999       // lo maximo que muestra el HUD

// Agrega la columna users.dinero si la base es anterior a este sistema
Dinero_PrepararTabla()
{
    MySQL_AsegurarColumna("users", "dinero", "INT NOT NULL DEFAULT 0 AFTER score");
}

Dinero_Obtener(playerid)
{
    return gJugador[playerid][jDinero];
}

// Suma (o resta, con monto negativo); queda entre 0 y DINERO_MAXIMO.
// Se compara antes de sumar para que un monto enorme no desborde el entero.
Dinero_Dar(playerid, monto)
{
    new actual = gJugador[playerid][jDinero];
    if (monto > 0 && actual > DINERO_MAXIMO - monto) actual = DINERO_MAXIMO;
    else if (monto < 0 && actual < -monto) actual = 0;
    else actual += monto;

    gJugador[playerid][jDinero] = actual;
    Dinero_Sincronizar(playerid);
}

// Muestra en el HUD el valor del servidor (al morir el GTA descuenta $100 por su cuenta)
Dinero_Sincronizar(playerid)
{
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, gJugador[playerid][jDinero]);
}
