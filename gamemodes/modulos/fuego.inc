// ------------------------------------------------------------
//  Manos de fuego del CEO, con un ajuste por postura.
//
//  El modelo 18693 tiene la llama ~1.5 m corrida de su origen: cuando la mano
//  gira, la llama sale despedida. Por eso cada postura tiene su propio ajuste:
//    - manos sueltas (tambien armas de una mano sin apuntar)
//    - sosteniendo cada arma de dos manos (cada una se agarra distinto)
//    - apuntando con cada arma
//  Una postura sin calibrar usa el ajuste del AK-47 (ver AjusteAUsar). Al apuntar queda bien
//  apuntando derecho; mirando muy arriba o abajo el brazo gira y se corre.
//
//  Se calibra en el juego con /editarfuego. Para las posturas de apuntar,
//  /poseapuntar congela una animacion de apuntar (no se puede apuntar y usar
//  el editor a la vez). Se guarda en la tabla fuego_ajustes: una fila por
//  postura y mano (arma 0 = manos sueltas, arma N = sosteniendo el arma N,
//  arma 100 + N = apuntando con el arma N).
// ------------------------------------------------------------

#define POSTURA_APUNTANDO       100

static const gClavePostura[] =
{
    0,                                                  // manos sueltas
    25, 27, 29, 30, 31, 33, 34, 35, 36, 37, 38,         // sosteniendo armas de dos manos
    122, 123, 124, 125, 126, 127, 128, 129, 130,        // apuntando (100 + id del arma)
    131, 132, 133, 134, 135, 136, 137, 138
};
#define TOTAL_POSTURAS          sizeof(gClavePostura)
#define SIN_FUEGO               -1      // mano sin particula
#define FUEGO_DESCONOCIDO       -2      // obliga a volver a pegarla

// Animaciones candidatas para simular la pose de apuntar (/poseapuntar N).
// Ninguna es identica a la pose real: se prueba cual se parece mas para cada arma.
static const gPoseLibreria[][] = { "RIFLE", "BUDDY", "PED", "COLT45", "PYTHON", "UZI", "TEC", "SHOP", "SNIPER", "FLAME" };
static const gPoseAnimacion[][] = { "RIFLE_fire", "buddy_fire", "GunMove_FWD", "colt45_fire", "python_fire", "UZI_fire", "TEC_fire", "SHP_Gun_Aim", "WEAPON_sniper", "FLAME_fire" };

// Ajuste inicial de "manos sueltas" (calibrado en el juego); el resto arranca sin calibrar
static const Float:gAjusteManosSueltas[2][9] =
{
    { 1.504, 0.483, -0.897,   0.0, -68.1, -49.8, 1.0, 1.0, 1.0 },
    { 1.680, 0.722,  0.324, 141.6, -80.7,   9.6, 1.0, 1.0, 1.0 }
};

// [postura][mano 0 = izq, 1 = der][x, y, z, rotX, rotY, rotZ, escX, escY, escZ]
static Float:gAjuste[TOTAL_POSTURAS][2][9];
static bool:gCalibrado[TOTAL_POSTURAS][2];

static gMostrada[MAX_PLAYERS][2];       // postura que se ve en cada mano (o SIN_FUEGO)
static bool:gEditando[MAX_PLAYERS];
static gPosturaEditada[MAX_PLAYERS];
static gPoseApuntar[MAX_PLAYERS];       // 0 = sin pose; N = animacion candidata N

forward AlCargarAjustesFuego();
forward ReaplicarPose(playerid, contador);

Fuego_Iniciar()
{
    for (new i = 0; i < 9; i++)
    {
        gAjuste[0][0][i] = gAjusteManosSueltas[0][i];
        gAjuste[0][1][i] = gAjusteManosSueltas[1][i];
    }
    gCalibrado[0][0] = true;
    gCalibrado[0][1] = true;

    mysql_tquery(gMySQL,
        "CREATE TABLE IF NOT EXISTS fuego_ajustes (\
            arma TINYINT UNSIGNED NOT NULL, mano TINYINT UNSIGNED NOT NULL,\
            x FLOAT NOT NULL, y FLOAT NOT NULL, z FLOAT NOT NULL,\
            rx FLOAT NOT NULL, ry FLOAT NOT NULL, rz FLOAT NOT NULL,\
            sx FLOAT NOT NULL, sy FLOAT NOT NULL, sz FLOAT NOT NULL,\
            PRIMARY KEY (arma, mano)\
        ) ENGINE=InnoDB");
    mysql_tquery(gMySQL, "SELECT * FROM fuego_ajustes", "AlCargarAjustesFuego");
}

public AlCargarAjustesFuego()
{
    new filas = cache_num_rows(), clave, mano, postura, cargados;
    for (new f = 0; f < filas; f++)
    {
        cache_get_value_name_int(f, "arma", clave);
        cache_get_value_name_int(f, "mano", mano);
        postura = PosturaDeClave(clave);
        if (postura == -1 || mano < 0 || mano > 1) continue;

        cache_get_value_name_float(f, "x", gAjuste[postura][mano][0]);
        cache_get_value_name_float(f, "y", gAjuste[postura][mano][1]);
        cache_get_value_name_float(f, "z", gAjuste[postura][mano][2]);
        cache_get_value_name_float(f, "rx", gAjuste[postura][mano][3]);
        cache_get_value_name_float(f, "ry", gAjuste[postura][mano][4]);
        cache_get_value_name_float(f, "rz", gAjuste[postura][mano][5]);
        cache_get_value_name_float(f, "sx", gAjuste[postura][mano][6]);
        cache_get_value_name_float(f, "sy", gAjuste[postura][mano][7]);
        cache_get_value_name_float(f, "sz", gAjuste[postura][mano][8]);
        gCalibrado[postura][mano] = true;
        cargados++;
    }
    printf("[Fuego] %d ajustes de manos de fuego cargados.", cargados);
    return 1;
}

// Indice de la postura con esa clave, o -1
static PosturaDeClave(clave)
{
    for (new i = 0; i < TOTAL_POSTURAS; i++)
    {
        if (gClavePostura[i] == clave) return i;
    }
    return -1;
}

// "manos sueltas", "AK47", "apuntando con AK47"...
static NombrePostura(postura, destino[], largo = sizeof(destino))
{
    new clave = gClavePostura[postura], arma[32];
    if (clave == 0)
    {
        format(destino, largo, "manos sueltas");
    }
    else if (clave < POSTURA_APUNTANDO)
    {
        GetWeaponName(WEAPON:clave, arma);
        format(destino, largo, "%s", arma);
    }
    else
    {
        GetWeaponName(WEAPON:(clave - POSTURA_APUNTANDO), arma);
        format(destino, largo, "apuntando con %s", arma);
    }
}

// Postura actual del jugador, o -1 si no tiene (en vehiculo, apuntando con los punos...)
static PosturaActual(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return -1;

    new arma = _:GetPlayerWeapon(playerid);
    new KEY:teclas, arriba, costado;
    GetPlayerKeys(playerid, teclas, arriba, costado);

    // Disparar sin apuntar tambien levanta los brazos como al apuntar (con los
    // punos KEY_FIRE es golpear: ahi sigue como manos sueltas)
    new apuntando = PosturaDeClave(POSTURA_APUNTANDO + arma);
    if (gPoseApuntar[playerid] || (teclas & KEY_AIM) || ((teclas & KEY_FIRE) && apuntando != -1)) return apuntando;

    new postura = PosturaDeClave(arma);
    return (postura == -1) ? 0 : postura;   // armas de una mano: como manos sueltas
}

static SlotDeMano(mano)  { return (mano == 0) ? SLOT_FUEGO_IZQ : SLOT_FUEGO_DER; }
static HuesoDeMano(mano) { return (mano == 0) ? HUESO_MANO_IZQ : HUESO_MANO_DER; }

static PegarFuego(playerid, mano, postura)
{
    SetPlayerAttachedObject(playerid, SlotDeMano(mano), MODELO_FUEGO, HuesoDeMano(mano),
        gAjuste[postura][mano][0], gAjuste[postura][mano][1], gAjuste[postura][mano][2],
        gAjuste[postura][mano][3], gAjuste[postura][mano][4], gAjuste[postura][mano][5],
        gAjuste[postura][mano][6], gAjuste[postura][mano][7], gAjuste[postura][mano][8]);
}

// Que ajuste usar para una postura: el propio si esta calibrada; si no, el del
// AK-47 (sosteniendo o apuntando, segun corresponda) para cualquier arma; y si
// tampoco hay, el de manos sueltas. Asi el fuego no desaparece con armas sin calibrar.
#define ARMA_REFERENCIA         30      // AK-47

static AjusteAUsar(postura, mano)
{
    if (postura == -1) return SIN_FUEGO;
    if (gCalibrado[postura][mano]) return postura;

    new clave = gClavePostura[postura];
    if (clave != 0)
    {
        new referencia = PosturaDeClave((clave >= POSTURA_APUNTANDO) ? (POSTURA_APUNTANDO + ARMA_REFERENCIA) : ARMA_REFERENCIA);
        if (referencia != -1 && gCalibrado[referencia][mano]) return referencia;
    }
    return gCalibrado[0][mano] ? 0 : SIN_FUEGO;
}

// Pega, cambia o saca el fuego de cada mano segun la postura actual
static ActualizarFuego(playerid)
{
    if (gEditando[playerid]) return;

    new postura = PosturaActual(playerid);
    for (new mano = 0; mano < 2; mano++)
    {
        new deseada = AjusteAUsar(postura, mano);
        if (deseada == gMostrada[playerid][mano]) continue;

        if (deseada == SIN_FUEGO) RemovePlayerAttachedObject(playerid, SlotDeMano(mano));
        else PegarFuego(playerid, mano, deseada);
        gMostrada[playerid][mano] = deseada;
    }
}

static RedibujarFuego(playerid)
{
    gMostrada[playerid][0] = FUEGO_DESCONOCIDO;
    gMostrada[playerid][1] = FUEGO_DESCONOCIDO;
    ActualizarFuego(playerid);
}

// Al spawnear el CEO (tambien despues de morir, /cure y /virus)
Fuego_Encender(playerid)
{
    gEditando[playerid] = false;
    gPoseApuntar[playerid] = 0;
    RedibujarFuego(playerid);
}

// Se llama en cada OnPlayerUpdate
Fuego_AlActualizar(playerid)
{
    if (!EsCEO(playerid) || !gJugador[playerid][jEnJuego]) return;
    ActualizarFuego(playerid);
}

Fuego_AlSalir(playerid)
{
    gEditando[playerid] = false;
    gPoseApuntar[playerid] = 0;
    gMostrada[playerid][0] = SIN_FUEGO;
    gMostrada[playerid][1] = SIN_FUEGO;
}

// ------------------------------------------------------------
//  Comandos de calibracion (solo el CEO)
// ------------------------------------------------------------

// Devuelve 1 si el comando era de este modulo
Fuego_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    new tipo;
    if (!strcmp(comando, "/editarfuego", true)) tipo = 1;
    else if (!strcmp(comando, "/poseapuntar", true)) tipo = 2;
    else if (!strcmp(comando, "/ajustarfuego", true)) tipo = 3;
    else return 0;

    if (!EsCEO(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }
    switch (tipo)
    {
        case 1: return ComandoEditarFuego(playerid, cmdtext);
        case 2: return ComandoPoseApuntar(playerid, cmdtext);
    }
    return ComandoAjustarFuego(playerid, cmdtext);
}

// Ajuste del que se parte al calibrar una postura: el mismo que se esta viendo
static OrigenDeAjuste(postura, mano)
{
    new origen = AjusteAUsar(postura, mano);
    return (origen == SIN_FUEGO) ? 0 : origen;
}

static GuardarAjuste(postura, mano)
{
    new consulta[256];
    mysql_format(gMySQL, consulta, sizeof(consulta),
        "REPLACE INTO fuego_ajustes VALUES (%d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
        gClavePostura[postura], mano,
        gAjuste[postura][mano][0], gAjuste[postura][mano][1], gAjuste[postura][mano][2],
        gAjuste[postura][mano][3], gAjuste[postura][mano][4], gAjuste[postura][mano][5],
        gAjuste[postura][mano][6], gAjuste[postura][mano][7], gAjuste[postura][mano][8]);
    mysql_tquery(gMySQL, consulta);
}

// /ajustarfuego [izq|der] [x|y|z|rx|ry|rz] [cantidad]: ajuste fino del fuego al
// APUNTAR con el arma que tenes en la mano. Se corrige, se apunta de verdad para
// ver como quedo y se repite (la pose real de apuntar no se puede congelar).
static ComandoAjustarFuego(playerid, const cmdtext[])
{
    new tmano[8], teje[8], tcantidad[16];
    ObtenerParametro(cmdtext, 1, tmano);
    ObtenerParametro(cmdtext, 2, teje);
    ObtenerParametro(cmdtext, 3, tcantidad);

    new mano = -1, eje = -1;
    if (!strcmp(tmano, "izq", true)) mano = 0;
    else if (!strcmp(tmano, "der", true)) mano = 1;

    static const nombresEje[][] = { "x", "y", "z", "rx", "ry", "rz" };
    for (new i = 0; i < sizeof(nombresEje); i++)
    {
        if (!strcmp(teje, nombresEje[i], true) && teje[0] != EOS) { eje = i; break; }
    }

    if (mano == -1 || eje == -1 || tcantidad[0] == EOS)
    {
        SendClientMessage(playerid, COLOR_INFO, "Uso: /ajustarfuego [izq|der] [x|y|z|rx|ry|rz] [cantidad]. Ej: /ajustarfuego izq x 0.1 o /ajustarfuego der rz -15");
        SendClientMessage(playerid, COLOR_INFO, "Ajusta el fuego al APUNTAR con el arma que tenes. Despues apunta para ver como quedo.");
        return 1;
    }

    new postura = PosturaDeClave(POSTURA_APUNTANDO + _:GetPlayerWeapon(playerid));
    if (postura == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Tenes que tener un arma de fuego en la mano.");
        return 1;
    }

    // Si no tenia ajuste propio se copia el de partida antes de corregir
    new origen = OrigenDeAjuste(postura, mano);
    if (origen != postura)
    {
        for (new i = 0; i < 9; i++) gAjuste[postura][mano][i] = gAjuste[origen][mano][i];
    }
    gAjuste[postura][mano][eje] += floatstr(tcantidad);
    gCalibrado[postura][mano] = true;
    GuardarAjuste(postura, mano);
    RedibujarFuego(playerid);

    new nombre[48], mensaje[144];
    NombrePostura(postura, nombre);
    format(mensaje, sizeof(mensaje), "%s, mano %s: x %.3f  y %.3f  z %.3f  rx %.1f  ry %.1f  rz %.1f", nombre, (mano == 0) ? ("izq") : ("der"),
        gAjuste[postura][mano][0], gAjuste[postura][mano][1], gAjuste[postura][mano][2],
        gAjuste[postura][mano][3], gAjuste[postura][mano][4], gAjuste[postura][mano][5]);
    SendClientMessage(playerid, COLOR_EXITO, mensaje);
    return 1;
}

// /editarfuego [izq|der]: ajusta la postura actual con el editor del juego
static ComandoEditarFuego(playerid, const cmdtext[])
{
    new texto[8], mano;
    ObtenerParametro(cmdtext, 1, texto);
    if (!strcmp(texto, "izq", true)) mano = 0;
    else if (!strcmp(texto, "der", true)) mano = 1;
    else
    {
        SendClientMessage(playerid, COLOR_INFO, "Uso: /editarfuego [izq|der]. Ajusta la postura actual (para apuntar, primero /poseapuntar).");
        return 1;
    }

    new postura = PosturaActual(playerid);
    if (postura == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Esta postura no tiene fuego: tenes que estar a pie (y con un arma si es para apuntar).");
        return 1;
    }

    gEditando[playerid] = true;
    gPosturaEditada[playerid] = postura;
    PegarFuego(playerid, mano, OrigenDeAjuste(postura, mano));
    EditAttachedObject(playerid, SlotDeMano(mano));

    new nombre[48], mensaje[128];
    NombrePostura(postura, nombre);
    format(mensaje, sizeof(mensaje), "Ajustando mano %s para: %s. Guarda con el disquete, ESC cancela.",
        (mano == 0) ? ("izq") : ("der"), nombre);
    SendClientMessage(playerid, COLOR_INFO, mensaje);
    return 1;
}

// /poseapuntar [N]: congela una animacion de apuntar para calibrar; 0 la saca
static ComandoPoseApuntar(playerid, const cmdtext[])
{
    new texto[8];
    ObtenerParametro(cmdtext, 1, texto);
    if (texto[0] == EOS)
    {
        new mensaje[144];
        format(mensaje, sizeof(mensaje), "Uso: /poseapuntar [1-%d] para probar poses de apuntar, 0 para salir. Con el arma en la mano.",
            sizeof(gPoseLibreria));
        SendClientMessage(playerid, COLOR_INFO, mensaje);
        return 1;
    }

    new pose = strval(texto);
    if (pose == 0)
    {
        gPoseApuntar[playerid] = 0;
        ClearAnimations(playerid, SYNC_ALL);
        RedibujarFuego(playerid);
        SendClientMessage(playerid, COLOR_INFO, "Pose de apuntar quitada.");
        return 1;
    }
    if (pose < 1 || pose > sizeof(gPoseLibreria))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ese numero de pose no existe.");
        return 1;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || PosturaDeClave(POSTURA_APUNTANDO + _:GetPlayerWeapon(playerid)) == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Tenes que estar a pie y con un arma de fuego en la mano.");
        return 1;
    }

    gPoseApuntar[playerid] = pose;
    AplicarPose(playerid);
    // La primera vez que se usa una libreria de animaciones puede no cargar: se repite
    SetTimerEx("ReaplicarPose", 300, false, "dd", playerid, gContadorConsulta[playerid]);
    RedibujarFuego(playerid);

    new mensaje[128];
    format(mensaje, sizeof(mensaje), "Pose %d (%s/%s). No te muevas. Ahora usa /editarfuego izq y der. /poseapuntar 0 para salir.",
        pose, gPoseLibreria[pose - 1], gPoseAnimacion[pose - 1]);
    SendClientMessage(playerid, COLOR_INFO, mensaje);
    return 1;
}

static AplicarPose(playerid)
{
    new i = gPoseApuntar[playerid] - 1;
    // Sin bucle y congelada en el ultimo cuadro, sin desplazar al jugador
    ApplyAnimation(playerid, gPoseLibreria[i], gPoseAnimacion[i], 4.1, false, true, true, true, 0, SYNC_ALL);
}

public ReaplicarPose(playerid, contador)
{
    if (contador == gContadorConsulta[playerid] && gPoseApuntar[playerid]) AplicarPose(playerid);
    return 1;
}

// Resultado del editor: guarda el ajuste en la postura que se estaba editando
Fuego_AlEditarObjeto(playerid, EDIT_RESPONSE:respuesta, slot,
    Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:sx, Float:sy, Float:sz)
{
    if (slot != SLOT_FUEGO_IZQ && slot != SLOT_FUEGO_DER) return 0;
    // Los ajustes son globales: un cliente modificado podria mandar una edicion falsa
    if (!EsCEO(playerid) || !gEditando[playerid]) return 1;

    new mano = (slot == SLOT_FUEGO_IZQ) ? 0 : 1, postura = gPosturaEditada[playerid];
    if (respuesta == EDIT_RESPONSE_FINAL)
    {
        gAjuste[postura][mano][0] = x;  gAjuste[postura][mano][1] = y;  gAjuste[postura][mano][2] = z;
        gAjuste[postura][mano][3] = rx; gAjuste[postura][mano][4] = ry; gAjuste[postura][mano][5] = rz;
        gAjuste[postura][mano][6] = sx; gAjuste[postura][mano][7] = sy; gAjuste[postura][mano][8] = sz;
        gCalibrado[postura][mano] = true;
        GuardarAjuste(postura, mano);

        new nombre[48], mensaje[112];
        NombrePostura(postura, nombre);
        format(mensaje, sizeof(mensaje), "Fuego de la mano %s guardado para: %s.", (mano == 0) ? ("izq") : ("der"), nombre);
        SendClientMessage(playerid, COLOR_EXITO, mensaje);
    }

    // Se vuelve a dibujar segun la postura actual (al cancelar, con el ajuste anterior)
    gEditando[playerid] = false;
    gMostrada[playerid][mano] = FUEGO_DESCONOCIDO;
    ActualizarFuego(playerid);
    return 1;
}
