// ------------------------------------------------------------
//  /id [id]: cartel con los datos de un jugador (o de uno mismo).
//  Lo puede usar cualquiera.
// ------------------------------------------------------------

// Devuelve 1 si el comando era de este modulo
Perfil_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/id", true)) return 0;

    new texto[12], objetivo = playerid;
    ObtenerParametro(cmdtext, 1, texto);
    if (texto[0] != EOS) objetivo = strval(texto);

    if (!IsPlayerConnected(objetivo) || !gJugador[objetivo][jLogueado])
    {
        SendClientMessage(playerid, COLOR_ERROR, "No hay ningun jugador conectado con ese id.");
        return 1;
    }
    MostrarPerfil(playerid, objetivo);
    return 1;
}

static MostrarPerfil(playerid, objetivo)
{
    new nombre[MAX_PLAYER_NAME + 1], rango[40], vip[24], dinero[20], bando[8], colorBando[12], titulo[48], cuerpo[512];
    GetPlayerName(objetivo, nombre, sizeof(nombre));
    Admin_EtiquetaRango(Admin_Rango(objetivo), rango);
    FormatearDinero(Dinero_Obtener(objetivo), dinero);
    Bando_Nombre(gJugador[objetivo][jBando], bando);
    if (Nemesis_Es(objetivo)) bando = "NEMESIS";
    Bando_EmbedNick(objetivo, colorBando);

    if (gJugador[objetivo][jVipNivel] > 0) format(vip, sizeof(vip), "{FFD700}Nivel %d", gJugador[objetivo][jVipNivel]);
    else vip = "{AAAAAA}Sin VIP";

    format(titulo, sizeof(titulo), "Datos de %s (ID %d)", nombre, objetivo);
    format(cuerpo, sizeof(cuerpo),
        "{AAAAAA}Nombre:\t\t{FFFFFF}%s\n\
        {AAAAAA}ID:\t\t\t{FFFFFF}%d\n\
        {AAAAAA}Bando:\t\t%s%s\n\
        {AAAAAA}Rango:\t\t%s\n\
        {AAAAAA}Nivel VIP:\t\t%s\n\
        {AAAAAA}Dinero:\t\t{00CC00}%s\n\
        {AAAAAA}Score:\t\t{FFFFFF}%d",
        nombre, objetivo,
        colorBando, bando,
        rango, vip, dinero, GetPlayerScore(objetivo));
    ShowPlayerDialog(playerid, DIALOGO_PERFIL, DIALOG_STYLE_MSGBOX, titulo, cuerpo, "Cerrar", "");
}
