// ------------------------------------------------------------
//  Utilidades compartidas
// ------------------------------------------------------------

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
