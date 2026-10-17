// ------------------------------------------------------------
//  Admins: rangos, skin y manos de fuego del CEO, y comandos.
//  El login RCON no da ningun poder dentro del juego.
// ------------------------------------------------------------

forward RegenerarAdmins();

// Posicion de las particulas sobre cada mano (0 = izquierda, 1 = derecha):
// x, y, z, rotX, rotY, rotZ, escalaX, escalaY, escalaZ. El modelo 18693 tiene el
// origen corrido ~1.5 m de la llama, por eso los valores grandes. Ajustados en el
// juego con /editarfuego (manos sueltas, sin arma de dos manos).
static Float:gFuegoAjuste[2][9] =
{
    { 1.504, 0.483, -0.897,   0.0, -68.1, -49.8, 1.0, 1.0, 1.0 },
    { 1.680, 0.722,  0.324, 141.6, -80.7,   9.6, 1.0, 1.0, 1.0 }
};

// Si las llamas estan pegadas ahora (se esconden al apuntar o con armas de dos manos)
static bool:gFuegoVisible[MAX_PLAYERS];

// Timer de regeneracion para los admins con manos de fuego
Admin_Iniciar()
{
    SetTimer("RegenerarAdmins", REGEN_INTERVALO, true);
}

// Al spawnear (tambien despues de morir) el CEO tiene su skin y su fuego
Admin_AlSpawnear(playerid)
{
    if (!EsCEO(playerid)) return;

    gJugador[playerid][jSkin] = SKIN_ADMIN;
    SetPlayerSkin(playerid, SKIN_ADMIN);
    PonerManosDeFuego(playerid);
}

static PonerManosDeFuego(playerid)
{
    PegarFuego(playerid, SLOT_FUEGO_IZQ, HUESO_MANO_IZQ, 0);
    PegarFuego(playerid, SLOT_FUEGO_DER, HUESO_MANO_DER, 1);
    gFuegoVisible[playerid] = true;
}

static SacarManosDeFuego(playerid)
{
    RemovePlayerAttachedObject(playerid, SLOT_FUEGO_IZQ);
    RemovePlayerAttachedObject(playerid, SLOT_FUEGO_DER);
    gFuegoVisible[playerid] = false;
}

static bool:EsArmaDeDosManos(WEAPON:arma)
{
    switch (arma)
    {
        case WEAPON_SHOTGUN, WEAPON_SHOTGSPA, WEAPON_MP5, WEAPON_AK47, WEAPON_M4,
             WEAPON_RIFLE, WEAPON_SNIPER, WEAPON_ROCKETLAUNCHER, WEAPON_HEATSEEKER,
             WEAPON_FLAMETHROWER, WEAPON_MINIGUN:
            return true;
    }
    return false;
}

// Al girar la mano (apuntar o sostener un arma de dos manos) la llama se
// desplaza lejos porque esta a ~1.5 m del hueso: en esos casos se esconde.
Admin_AlActualizar(playerid)
{
    if (!EsCEO(playerid) || !gJugador[playerid][jEnJuego]) return;
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return;

    new KEY:teclas, arriba, costado;
    GetPlayerKeys(playerid, teclas, arriba, costado);
    new bool:mostrar = !(teclas & KEY_AIM) && !EsArmaDeDosManos(GetPlayerWeapon(playerid));

    if (mostrar && !gFuegoVisible[playerid]) PonerManosDeFuego(playerid);
    else if (!mostrar && gFuegoVisible[playerid]) SacarManosDeFuego(playerid);
}

static PegarFuego(playerid, slot, hueso, mano)
{
    SetPlayerAttachedObject(playerid, slot, MODELO_FUEGO, hueso,
        gFuegoAjuste[mano][0], gFuegoAjuste[mano][1], gFuegoAjuste[mano][2],
        gFuegoAjuste[mano][3], gFuegoAjuste[mano][4], gFuegoAjuste[mano][5],
        gFuegoAjuste[mano][6], gFuegoAjuste[mano][7], gFuegoAjuste[mano][8]);
}

// Cada REGEN_INTERVALO: +10 de vida y +10 de chaleco, hasta 100
public RegenerarAdmins()
{
    new Float:vida, Float:chaleco;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i) || !EsCEO(i) || !gJugador[i][jEnJuego]) continue;

        new PLAYER_STATE:estado = GetPlayerState(i);
        if (estado == PLAYER_STATE_WASTED || estado == PLAYER_STATE_NONE || estado == PLAYER_STATE_SPECTATING) continue;

        GetPlayerHealth(i, vida);
        GetPlayerArmour(i, chaleco);
        if (vida < 100.0)
        {
            vida += REGEN_CANTIDAD;
            SetPlayerHealth(i, (vida > 100.0) ? 100.0 : vida);
        }
        if (chaleco < 100.0)
        {
            chaleco += REGEN_CANTIDAD;
            SetPlayerArmour(i, (chaleco > 100.0) ? 100.0 : chaleco);
        }
    }
    return 1;
}

// Resultado de /editarfuego: guarda el ajuste de la mano editada (dura hasta reiniciar)
Admin_AlEditarObjeto(playerid, EDIT_RESPONSE:respuesta, slot,
    Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:sx, Float:sy, Float:sz)
{
    if (slot != SLOT_FUEGO_IZQ && slot != SLOT_FUEGO_DER) return 0;
    // El ajuste es global: un cliente modificado podria mandar una edicion falsa
    if (!EsCEO(playerid)) return 1;

    new mano = (slot == SLOT_FUEGO_IZQ) ? 0 : 1;
    if (respuesta == EDIT_RESPONSE_FINAL)
    {
        gFuegoAjuste[mano][0] = x;  gFuegoAjuste[mano][1] = y;  gFuegoAjuste[mano][2] = z;
        gFuegoAjuste[mano][3] = rx; gFuegoAjuste[mano][4] = ry; gFuegoAjuste[mano][5] = rz;
        gFuegoAjuste[mano][6] = sx; gFuegoAjuste[mano][7] = sy; gFuegoAjuste[mano][8] = sz;

        // Se muestran para dejarlos fijos en el codigo
        new mensaje[144];
        format(mensaje, sizeof(mensaje), "Fuego mano %s: { %.3f, %.3f, %.3f, %.1f, %.1f, %.1f, %.2f, %.2f, %.2f }",
            (mano == 0) ? ("izq") : ("der"), x, y, z, rx, ry, rz, sx, sy, sz);
        SendClientMessage(playerid, COLOR_INFO, mensaje);
        printf("[Admin] %s", mensaje);
    }

    // Se vuelve a pegar con el ajuste guardado (al cancelar, el anterior)
    PegarFuego(playerid, slot, (mano == 0) ? HUESO_MANO_IZQ : HUESO_MANO_DER, mano);
    return 1;
}

// Devuelve 1 si el comando era de este modulo
Admin_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (!strcmp(comando, "/admin", true) || !strcmp(comando, "/admins", true)) return ComandoListaAdmins(playerid);
    if (!strcmp(comando, "/daradmin", true)) return ComandoDarAdmin(playerid, cmdtext);
    if (!strcmp(comando, "/dardinero", true)) return ComandoDarDinero(playerid, cmdtext);
    if (!strcmp(comando, "/editarfuego", true)) return ComandoEditarFuego(playerid, cmdtext);

    new bool:esCura = !strcmp(comando, "/cure", true);
    if (!esCura && strcmp(comando, "/virus", true)) return 0;

    if (!EsCEO(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }

    // /cure [id] y /virus [id]: sin id se aplica a uno mismo
    new pid[12], objetivo = playerid;
    ObtenerParametro(cmdtext, 1, pid);
    if (pid[0] != EOS) objetivo = strval(pid);

    if (!IsPlayerConnected(objetivo) || !gJugador[objetivo][jEnJuego])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ese jugador no esta en juego.");
        return 1;
    }
    if (esCura)
    {
        if (gJugador[objetivo][jBando] == BANDO_HUMANO)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Ese jugador ya es humano.");
            return 1;
        }
        Bando_Curar(objetivo);
    }
    else
    {
        if (gJugador[objetivo][jBando] == BANDO_ZOMBIE)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Ese jugador ya es zombie.");
            return 1;
        }
        Bando_Infectar(objetivo);
    }

    if (EsCEO(objetivo)) Admin_AlSpawnear(objetivo);
    return 1;
}

// ------------------------------------------------------------
//  Rangos
// ------------------------------------------------------------

// Rango guardado en la base (users.admin_nivel)
Admin_Rango(playerid)
{
    return gJugador[playerid][jAdminNivel];
}

// "[CEO]" con su color, para insertar en un texto
static Admin_EtiquetaRango(rango, destino[], largo = sizeof(destino))
{
    switch (rango)
    {
        case RANGO_MODERADOR:     format(destino, largo, EMBED_MODERADOR "[Moderador]");
        case RANGO_ADMINISTRADOR: format(destino, largo, EMBED_ADMINISTRADOR "[Administrador]");
        case RANGO_CEO:           format(destino, largo, EMBED_CEO "[CEO]");
        default:                  format(destino, largo, "{FFFFFF}[Usuario]");
    }
}

// /admin: cartel con los admins conectados, del rango mas alto al mas bajo
static ComandoListaAdmins(playerid)
{
    new cuerpo[2048], linea[96], etiqueta[40], nombre[MAX_PLAYER_NAME + 1], total;

    for (new rango = RANGO_CEO; rango >= RANGO_MODERADOR; rango--)
    {
        for (new i = 0; i < MAX_PLAYERS; i++)
        {
            if (!IsPlayerConnected(i) || !gJugador[i][jLogueado] || Admin_Rango(i) != rango) continue;

            GetPlayerName(i, nombre, sizeof(nombre));
            Admin_EtiquetaRango(rango, etiqueta);
            format(linea, sizeof(linea), "%s {FFFFFF}%s {AAAAAA}(ID: %d)\n", etiqueta, nombre, i);
            if (strlen(cuerpo) + strlen(linea) >= sizeof(cuerpo)) break;
            strcat(cuerpo, linea);
            total++;
        }
    }

    if (total == 0) cuerpo = "{FFFFFF}No hay administradores conectados.";

    new titulo[40];
    format(titulo, sizeof(titulo), "Admins conectados (%d)", total);
    ShowPlayerDialog(playerid, DIALOGO_ADMINS, DIALOG_STYLE_MSGBOX, titulo, cuerpo, "Cerrar", "");
    return 1;
}

// /daradmin [id] [nivel]: 0 quita, 1 Moderador, 2 Administrador. Solo el CEO.
// El rango CEO no se da desde el juego: solo se carga en la base.
static ComandoDarAdmin(playerid, const cmdtext[])
{
    if (!EsCEO(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo el CEO puede asignar rangos.");
        return 1;
    }

    new pid[12], pnivel[12];
    ObtenerParametro(cmdtext, 1, pid);
    ObtenerParametro(cmdtext, 2, pnivel);
    if (pid[0] == EOS || pnivel[0] == EOS)
    {
        SendClientMessage(playerid, COLOR_INFO, "Uso: /daradmin [id] [nivel]. 0 = quitar, 1 = Moderador, 2 = Administrador.");
        return 1;
    }

    new objetivo = strval(pid), nivel = strval(pnivel);
    if (!IsPlayerConnected(objetivo) || !gJugador[objetivo][jLogueado] || gJugador[objetivo][jId] == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ese jugador no inicio sesion.");
        return 1;
    }
    if (nivel < RANGO_NINGUNO || nivel > RANGO_ADMINISTRADOR)
    {
        SendClientMessage(playerid, COLOR_ERROR, "El nivel va de 0 a 2 (el rango CEO no se puede dar).");
        return 1;
    }
    if (EsCEO(objetivo))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No se puede cambiar el rango de un CEO desde el juego.");
        return 1;
    }

    gJugador[objetivo][jAdminNivel] = nivel;
    new consulta[96];
    mysql_format(gMySQL, consulta, sizeof(consulta), "UPDATE users SET admin_nivel = %d WHERE id = %d",
        nivel, gJugador[objetivo][jId]);
    mysql_tquery(gMySQL, consulta);

    new nombre[MAX_PLAYER_NAME + 1], etiqueta[40], mensaje[128];
    GetPlayerName(objetivo, nombre, sizeof(nombre));
    if (nivel == RANGO_NINGUNO)
    {
        format(mensaje, sizeof(mensaje), "Le quitaste el rango de admin a %s.", nombre);
        SendClientMessage(playerid, COLOR_EXITO, mensaje);
        if (objetivo != playerid) SendClientMessage(objetivo, COLOR_INFO, "Ya no tenes rango de admin.");
    }
    else
    {
        Admin_EtiquetaRango(nivel, etiqueta);
        format(mensaje, sizeof(mensaje), "{FFFFFF}%s ahora es %s{FFFFFF}.", nombre, etiqueta);
        SendClientMessage(playerid, COLOR_INFO, mensaje);
        if (objetivo != playerid)
        {
            format(mensaje, sizeof(mensaje), "{FFFFFF}Ahora sos %s{FFFFFF}.", etiqueta);
            SendClientMessage(objetivo, COLOR_INFO, mensaje);
        }
    }
    return 1;
}

// /editarfuego [izq|der]: mover las particulas con el mouse
static ComandoEditarFuego(playerid, const cmdtext[])
{
    if (!EsCEO(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }

    new mano[8];
    ObtenerParametro(cmdtext, 1, mano);
    if (!strcmp(mano, "izq", true)) EditAttachedObject(playerid, SLOT_FUEGO_IZQ);
    else if (!strcmp(mano, "der", true)) EditAttachedObject(playerid, SLOT_FUEGO_DER);
    else SendClientMessage(playerid, COLOR_INFO, "Uso: /editarfuego [izq|der]. Guarda con el disquete, ESC cancela.");
    return 1;
}

// /dardinero [id] [monto]: monto negativo para quitar
static ComandoDarDinero(playerid, const cmdtext[])
{
    if (!EsCEO(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }

    new pid[12], pmonto[16];
    ObtenerParametro(cmdtext, 1, pid);
    ObtenerParametro(cmdtext, 2, pmonto);
    if (pid[0] == EOS || pmonto[0] == EOS)
    {
        SendClientMessage(playerid, COLOR_INFO, "Uso: /dardinero [id] [monto]. Con monto negativo se quita dinero.");
        return 1;
    }

    new objetivo = strval(pid), monto = strval(pmonto);
    if (!IsPlayerConnected(objetivo) || !gJugador[objetivo][jLogueado])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ese jugador no esta conectado.");
        return 1;
    }

    Dinero_Dar(objetivo, monto);

    new nombre[MAX_PLAYER_NAME + 1], cantidad[20], total[20], mensaje[128];
    GetPlayerName(objetivo, nombre, sizeof(nombre));
    FormatearDinero(monto, cantidad);
    FormatearDinero(Dinero_Obtener(objetivo), total);
    format(mensaje, sizeof(mensaje), "Le diste %s a %s. Ahora tiene %s.", cantidad, nombre, total);
    SendClientMessage(playerid, COLOR_EXITO, mensaje);

    if (objetivo != playerid)
    {
        format(mensaje, sizeof(mensaje), "Un administrador te dio %s. Ahora tenes %s.", cantidad, total);
        SendClientMessage(objetivo, COLOR_EXITO, mensaje);
    }
    return 1;
}
