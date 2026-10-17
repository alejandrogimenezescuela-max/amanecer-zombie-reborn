// ------------------------------------------------------------
//  Utilidades compartidas
// ------------------------------------------------------------

// Neutraliza los codigos de color {RRGGBB} que escribe un jugador, para que
// no pueda imitar mensajes del servidor. Cambia { y } por ( y ).
LimpiarColores(texto[])
{
    for (new i = 0; texto[i] != EOS; i++)
    {
        if (texto[i] == '{') texto[i] = '(';
        else if (texto[i] == '}') texto[i] = ')';
    }
}

// 150000 -> "$150.000"
FormatearDinero(monto, destino[], largo = sizeof(destino))
{
    new numero[16], cifras, j = 0;
    format(numero, sizeof(numero), "%d", monto);
    cifras = strlen(numero);

    destino[j++] = '$';
    for (new i = 0; i < cifras && j < largo - 1; i++)
    {
        if (i > 0 && numero[i - 1] != '-' && (cifras - i) % 3 == 0) destino[j++] = '.';
        if (j < largo - 1) destino[j++] = numero[i];
    }
    destino[j] = EOS;
}

// Copia la palabra numero 'indice' (separadas por espacios) de texto.
// Sirve para leer los parametros de un comando: indice 0 es el comando.
ObtenerParametro(const texto[], indice, destino[], largo = sizeof(destino))
{
    new pos = 0, actual = 0, inicio;
    destino[0] = EOS;

    while (texto[pos] != EOS)
    {
        while (texto[pos] == ' ') pos++;
        if (texto[pos] == EOS) break;

        inicio = pos;
        while (texto[pos] != EOS && texto[pos] != ' ') pos++;

        if (actual == indice)
        {
            strmid(destino, texto, inicio, pos, largo);
            return;
        }
        actual++;
    }
}
