// ------------------------------------------------------------
//  Virus Nemesis: un pickup (calavera) que cualquier jugador puede
//  tocar para comprar el virus con score. El Nemesis es un zombie
//  potenciado: skin 17, nombre en rojo, RPG sin fin, 5000 de vida,
//  100 de chaleco y un efecto en el cuerpo. Deja de serlo al morir.
//
//  La vida de SA-MP no se sincroniza bien por encima de 255, asi que
//  los 5000 son una vida virtual: la vida real queda alta y cada golpe
//  se descuenta de la virtual (OnPlayerTakeDamage).
// ------------------------------------------------------------

#define MODELO_PICKUP_NEMESIS   1254    // calavera
#define PRECIO_NEMESIS_SCORE    5000
#define NEMESIS_COLOR           0xFF0000FF      // rojo puro
const Float:NEMESIS_VIDA = 5000.0;
const Float:NEMESIS_VIDA_REAL = 250.0;  // colchon para que un golpe fuerte no lo mate de una
const Float:NEMESIS_CHALECO = 100.0;
#define NEMESIS_MUNICION_RPG    9999
#define NEMESIS_RECARGA_MS      2000

#define MODELO_EFECTO_NEMESIS   18728
#define SLOT_EFECTO_NEMESIS     7
#define HUESO_TORSO             1

#define ESPERA_PICKUP_MS        5000    // para no reabrir el menu mientras sigue parado encima

const Float:PICKUP_NEMESIS_X = 2035.1262;
const Float:PICKUP_NEMESIS_Y = -1404.9484;
const Float:PICKUP_NEMESIS_Z = 17.2464;

// Posicion del efecto sobre el torso: x, y, z, rotX, rotY, rotZ, escX, escY, escZ.
// Se calibra con /editarnemesis y se guarda en efectos_ajustes (fila 'nemesis').
static Float:gAjusteEfecto[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
static bool:gEditandoEfecto[MAX_PLAYERS];

static gPickupNemesis;
static bool:gNemesis[MAX_PLAYERS];
static Float:gVidaNemesis[MAX_PLAYERS];
static gUltimoPickup[MAX_PLAYERS];
static PlayerText:gTextoVida[MAX_PLAYERS];

forward RecargarNemesis();
forward AlCargarEfectoNemesis();

Nemesis_Iniciar()
{
    // Tipo 1: siempre esta y avisa cada vez que se toca (sin efectos del juego)
    gPickupNemesis = CreatePickup(MODELO_PICKUP_NEMESIS, 1, PICKUP_NEMESIS_X, PICKUP_NEMESIS_Y, PICKUP_NEMESIS_Z, -1);
    SetTimer("RecargarNemesis", NEMESIS_RECARGA_MS, true);

    // Ajustes de efectos pegados al cuerpo (una fila por efecto: 'nemesis', y mas adelante 'star')
    mysql_tquery(gMySQL,
        "CREATE TABLE IF NOT EXISTS efectos_ajustes (\
            nombre VARCHAR(16) NOT NULL,\
            x FLOAT NOT NULL, y FLOAT NOT NULL, z FLOAT NOT NULL,\
            rx FLOAT NOT NULL, ry FLOAT NOT NULL, rz FLOAT NOT NULL,\
            sx FLOAT NOT NULL, sy FLOAT NOT NULL, sz FLOAT NOT NULL,\
            PRIMARY KEY (nombre)\
        ) ENGINE=InnoDB");
    mysql_tquery(gMySQL, "SELECT * FROM efectos_ajustes WHERE nombre = 'nemesis'", "AlCargarEfectoNemesis");
}

public AlCargarEfectoNemesis()
{
    if (cache_num_rows() == 0) return 1;

    static const columnas[][] = { "x", "y", "z", "rx", "ry", "rz", "sx", "sy", "sz" };
    for (new i = 0; i < sizeof(columnas); i++)
    {
        cache_get_value_name_float(0, columnas[i], gAjusteEfecto[i]);
    }
    print("[Nemesis] Ajuste del efecto cargado.");
    return 1;
}

Nemesis_Es(playerid)
{
    return gNemesis[playerid];
}

// ------------------------------------------------------------
//  Pickup y compra
// ------------------------------------------------------------

// Devuelve 1 si el pickup era el del Nemesis
Nemesis_AlTocarPickup(playerid, pickupid)
{
    if (pickupid != gPickupNemesis) return 0;
    if (!gJugador[playerid][jEnJuego]) return 1;

    new ahora = GetTickCount();
    if (gUltimoPickup[playerid] != 0 && ahora - gUltimoPickup[playerid] < ESPERA_PICKUP_MS) return 1;
    gUltimoPickup[playerid] = ahora;

    if (gNemesis[playerid])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ya sos Nemesis.");
        return 1;
    }

    new cuerpo[384];
    format(cuerpo, sizeof(cuerpo),
        "{FF0000}VIRUS NEMESIS\n\n\
        {FFFFFF}Te convierte en Nemesis: un zombie con %d de vida,\n\
        %d de chaleco y un RPG con municion infinita.\n\
        Dejas de ser Nemesis al morir.\n\n\
        {AAAAAA}Precio: {FFD700}%d de score\n\
        {AAAAAA}Tu score: {FFFFFF}%d",
        floatround(NEMESIS_VIDA), floatround(NEMESIS_CHALECO), PRECIO_NEMESIS_SCORE, GetPlayerScore(playerid));
    ShowPlayerDialog(playerid, DIALOGO_NEMESIS, DIALOG_STYLE_MSGBOX, "Virus Nemesis", cuerpo, "Comprar", "Cancelar");
    return 1;
}

// Devuelve 1 si el dialogo era de este modulo
Nemesis_AlResponderDialogo(playerid, dialogid, response)
{
    if (dialogid != DIALOGO_NEMESIS) return 0;
    if (!response || !gJugador[playerid][jEnJuego] || gNemesis[playerid]) return 1;

    new score = GetPlayerScore(playerid);
    if (score < PRECIO_NEMESIS_SCORE)
    {
        new mensaje[96];
        format(mensaje, sizeof(mensaje), "No te alcanza: el virus Nemesis cuesta %d de score y tenes %d.", PRECIO_NEMESIS_SCORE, score);
        SendClientMessage(playerid, COLOR_ERROR, mensaje);
        return 1;
    }

    SetPlayerScore(playerid, score - PRECIO_NEMESIS_SCORE);
    Convertir(playerid);
    return 1;
}

static Convertir(playerid)
{
    gNemesis[playerid] = true;
    gVidaNemesis[playerid] = NEMESIS_VIDA;

    // Es un zombie, pero potenciado
    gJugador[playerid][jBando] = BANDO_ZOMBIE;
    gJugador[playerid][jSkin] = SKIN_NEMESIS;
    SetPlayerSkin(playerid, SKIN_NEMESIS);
    SetPlayerColor(playerid, NEMESIS_COLOR);

    ResetPlayerWeapons(playerid);
    GivePlayerWeapon(playerid, WEAPON_ROCKETLAUNCHER, NEMESIS_MUNICION_RPG);
    SetPlayerHealth(playerid, NEMESIS_VIDA_REAL);
    SetPlayerArmour(playerid, NEMESIS_CHALECO);

    Nemesis_ActualizarLuz(playerid);
    CrearTextoVida(playerid);

    new nombre[MAX_PLAYER_NAME + 1], mensaje[80];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(mensaje, sizeof(mensaje), "[<!>] %s se convirtio en NEMESIS.", nombre);
    SendClientMessageToAll(NEMESIS_COLOR, mensaje);
    SendClientMessage(playerid, NEMESIS_COLOR, "Sos Nemesis. Caza a todos los humanos.");
}

// Deja de ser Nemesis (al morir, al curarlo o al salir)
Nemesis_Terminar(playerid)
{
    if (!gNemesis[playerid]) return;
    gNemesis[playerid] = false;
    gEditandoEfecto[playerid] = false;

    RemovePlayerAttachedObject(playerid, SLOT_EFECTO_NEMESIS);
    if (gTextoVida[playerid] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, gTextoVida[playerid]);
        gTextoVida[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }
}

// Al morir vuelve a ser un zombie comun, con un skin de zombie al azar
Nemesis_AlMorir(playerid)
{
    if (!gNemesis[playerid]) return;
    Nemesis_Terminar(playerid);
    gJugador[playerid][jSkin] = Bando_SkinEnIndice(BANDO_ZOMBIE, random(Bando_TotalSkins(BANDO_ZOMBIE)));
}

Nemesis_AlConectar(playerid)
{
    gNemesis[playerid] = false;
    gEditandoEfecto[playerid] = false;
    gUltimoPickup[playerid] = 0;
    gTextoVida[playerid] = INVALID_PLAYER_TEXT_DRAW;
}

// ------------------------------------------------------------
//  Vida virtual, efecto y RPG
// ------------------------------------------------------------

// Cada golpe se descuenta de la vida virtual y la vida real se vuelve a llenar.
// Mientras tenga chaleco el dano va al chaleco (lo maneja el juego).
Nemesis_AlRecibirDanio(playerid, Float:cantidad)
{
    if (!gNemesis[playerid]) return;

    new Float:chaleco;
    GetPlayerArmour(playerid, chaleco);
    if (chaleco > 0.0) return;

    gVidaNemesis[playerid] -= cantidad;
    if (gVidaNemesis[playerid] <= 0.0)
    {
        gVidaNemesis[playerid] = 0.0;
        SetPlayerHealth(playerid, 0.0);
    }
    else
    {
        SetPlayerHealth(playerid, NEMESIS_VIDA_REAL);
    }
    ActualizarTextoVida(playerid);
}

static CrearTextoVida(playerid)
{
    // Debajo del dinero, alineado a la derecha como el HUD
    new PlayerText:texto = CreatePlayerTextDraw(playerid, 608.0, 102.0, "_");
    PlayerTextDrawAlignment(playerid, texto, TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawFont(playerid, texto, TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, texto, 0.3, 1.3);
    PlayerTextDrawSetOutline(playerid, texto, 1);
    PlayerTextDrawColour(playerid, texto, NEMESIS_COLOR);
    gTextoVida[playerid] = texto;
    ActualizarTextoVida(playerid);
}

static ActualizarTextoVida(playerid)
{
    if (gTextoVida[playerid] == INVALID_PLAYER_TEXT_DRAW) return;
    new texto[32];
    format(texto, sizeof(texto), "NEMESIS ~w~%d", floatround(gVidaNemesis[playerid]));
    PlayerTextDrawSetString(playerid, gTextoVida[playerid], texto);
    PlayerTextDrawShow(playerid, gTextoVida[playerid]);
}

// Efecto en el centro del cuerpo (solo el Nemesis lo tiene); /luz lo oculta
Nemesis_ActualizarLuz(playerid)
{
    if (!gNemesis[playerid] || gEditandoEfecto[playerid]) return;
    if (gJugador[playerid][jSinLuz])
    {
        RemovePlayerAttachedObject(playerid, SLOT_EFECTO_NEMESIS);
        return;
    }
    SetPlayerAttachedObject(playerid, SLOT_EFECTO_NEMESIS, MODELO_EFECTO_NEMESIS, HUESO_TORSO,
        gAjusteEfecto[0], gAjusteEfecto[1], gAjusteEfecto[2],
        gAjusteEfecto[3], gAjusteEfecto[4], gAjusteEfecto[5],
        gAjusteEfecto[6], gAjusteEfecto[7], gAjusteEfecto[8]);
}

// ------------------------------------------------------------
//  /editarnemesis: calibrar el efecto (solo el CEO, siendo Nemesis)
// ------------------------------------------------------------

// Devuelve 1 si el comando era de este modulo
Nemesis_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/editarnemesis", true)) return 0;

    if (!EsCEO(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "No tenes permiso para usar este comando.");
        return 1;
    }
    if (!gNemesis[playerid])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Tenes que ser Nemesis para ver y ajustar el efecto.");
        return 1;
    }
    if (gJugador[playerid][jSinLuz])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Tenes los efectos ocultos: usa /luz para mostrarlos primero.");
        return 1;
    }

    gEditandoEfecto[playerid] = true;
    EditAttachedObject(playerid, SLOT_EFECTO_NEMESIS);
    SendClientMessage(playerid, COLOR_INFO, "Ajustando el efecto del Nemesis. Guarda con el disquete, ESC cancela.");
    return 1;
}

// Resultado del editor; devuelve 1 si era el efecto del Nemesis
Nemesis_AlEditarObjeto(playerid, EDIT_RESPONSE:respuesta, slot,
    Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:sx, Float:sy, Float:sz)
{
    if (slot != SLOT_EFECTO_NEMESIS) return 0;
    // El ajuste es global: un cliente modificado podria mandar una edicion falsa
    if (!EsCEO(playerid) || !gEditandoEfecto[playerid]) return 1;
    gEditandoEfecto[playerid] = false;

    if (respuesta == EDIT_RESPONSE_FINAL)
    {
        gAjusteEfecto[0] = x;  gAjusteEfecto[1] = y;  gAjusteEfecto[2] = z;
        gAjusteEfecto[3] = rx; gAjusteEfecto[4] = ry; gAjusteEfecto[5] = rz;
        gAjusteEfecto[6] = sx; gAjusteEfecto[7] = sy; gAjusteEfecto[8] = sz;

        new consulta[256];
        mysql_format(gMySQL, consulta, sizeof(consulta),
            "REPLACE INTO efectos_ajustes VALUES ('nemesis', %f, %f, %f, %f, %f, %f, %f, %f, %f)",
            x, y, z, rx, ry, rz, sx, sy, sz);
        mysql_tquery(gMySQL, consulta);
        SendClientMessage(playerid, COLOR_EXITO, "Efecto del Nemesis guardado.");
    }

    // Se vuelve a pegar con el ajuste guardado a todos los Nemesis (al cancelar, el anterior)
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && gNemesis[i]) Nemesis_ActualizarLuz(i);
    }
    return 1;
}

// RPG "infinito": se recarga si le quedan pocos cohetes
public RecargarNemesis()
{
    new WEAPON:arma, municion;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i) || !gNemesis[i]) continue;

        GetPlayerWeaponData(i, WEAPON_SLOT:7, arma, municion);
        if (arma != WEAPON_ROCKETLAUNCHER || municion < NEMESIS_MUNICION_RPG / 2)
        {
            GivePlayerWeapon(i, WEAPON_ROCKETLAUNCHER, NEMESIS_MUNICION_RPG - ((arma == WEAPON_ROCKETLAUNCHER) ? municion : 0));
        }
    }
    return 1;
}
