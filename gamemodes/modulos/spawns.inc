// ------------------------------------------------------------
//  Spawns por bando (tabla spawns) y herramientas de admin
//  para armarlos: teleport con clic en el mapa, /ir, /spawnh,
//  /spawnz, /spawns y /borrarspawn. Solo admins RCON.
// ------------------------------------------------------------

#define MAX_SPAWNS              40

enum E_SPAWN
{
    sId,
    E_BANDO:sBando,
    Float:sX,
    Float:sY,
    Float:sZ,
    Float:sAngulo
}

static gSpawns[MAX_SPAWNS][E_SPAWN];
static gTotalSpawns;

forward AlCargarSpawns();
forward AlGuardarSpawn(playerid, contador, bando, Float:x, Float:y, Float:z, Float:angulo);

// Crea la tabla si no existe y carga los spawns
Spawns_Cargar()
{
    mysql_tquery(gMySQL,
        "CREATE TABLE IF NOT EXISTS spawns (\
            id INT UNSIGNED NOT NULL AUTO_INCREMENT,\
            bando ENUM('humano', 'zombie') NOT NULL,\
            pos_x FLOAT NOT NULL, pos_y FLOAT NOT NULL, pos_z FLOAT NOT NULL,\
            angulo FLOAT NOT NULL DEFAULT 0,\
            PRIMARY KEY (id)\
        ) ENGINE=InnoDB");
    mysql_tquery(gMySQL, "SELECT id, bando, pos_x, pos_y, pos_z, angulo FROM spawns ORDER BY id", "AlCargarSpawns");
}

public AlCargarSpawns()
{
    new filas = cache_num_rows(), bando[8];
    gTotalSpawns = 0;

    for (new i = 0; i < filas && gTotalSpawns < MAX_SPAWNS; i++)
    {
        cache_get_value_name_int(i, "id", gSpawns[gTotalSpawns][sId]);
        cache_get_value_name(i, "bando", bando);
        gSpawns[gTotalSpawns][sBando] = (strcmp(bando, "zombie") == 0) ? BANDO_ZOMBIE : BANDO_HUMANO;
        cache_get_value_name_float(i, "pos_x", gSpawns[gTotalSpawns][sX]);
        cache_get_value_name_float(i, "pos_y", gSpawns[gTotalSpawns][sY]);
        cache_get_value_name_float(i, "pos_z", gSpawns[gTotalSpawns][sZ]);
        cache_get_value_name_float(i, "angulo", gSpawns[gTotalSpawns][sAngulo]);
        gTotalSpawns++;
    }
    printf("[Spawns] %d spawns cargados.", gTotalSpawns);
    return 1;
}

// Spawn al azar del bando; si no hay ninguno cargado, Grove Street
Spawns_Elegir(E_BANDO:bando, &Float:x, &Float:y, &Float:z, &Float:angulo)
{
    new candidatos[MAX_SPAWNS], total;
    for (new i = 0; i < gTotalSpawns; i++)
    {
        if (gSpawns[i][sBando] == bando) candidatos[total++] = i;
    }

    if (total == 0)
    {
        x = SPAWN_INICIAL_X;
        y = SPAWN_INICIAL_Y;
        z = SPAWN_INICIAL_Z;
        angulo = SPAWN_INICIAL_ANGULO;
        return;
    }

    new elegido = candidatos[random(total)];
    x = gSpawns[elegido][sX];
    y = gSpawns[elegido][sY];
    z = gSpawns[elegido][sZ];
    angulo = gSpawns[elegido][sAngulo];
}

// ------------------------------------------------------------
//  Herramientas de admin
// ------------------------------------------------------------

// Teleport con clic derecho en el mapa del menu de pausa
Spawns_AlClicEnMapa(playerid, Float:x, Float:y, Float:z)
{
    if (!IsPlayerAdmin(playerid)) return;
    Teletransportar(playerid, x, y, z);
}

// Devuelve 1 si el comando era de este modulo
Spawns_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);

    if (strcmp(comando, "/ir", true) && strcmp(comando, "/spawnh", true) && strcmp(comando, "/spawnz", true)
        && strcmp(comando, "/spawns", true) && strcmp(comando, "/borrarspawn", true))
    {
        return 0;
    }

    if (!IsPlayerAdmin(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo los administradores pueden usar este comando.");
        return 1;
    }

    if (!strcmp(comando, "/ir", true))
    {
        new px[16], py[16], pz[16];
        ObtenerParametro(cmdtext, 1, px);
        ObtenerParametro(cmdtext, 2, py);
        ObtenerParametro(cmdtext, 3, pz);
        if (px[0] == EOS || py[0] == EOS)
        {
            SendClientMessage(playerid, COLOR_INFO, "Uso: /ir [x] [y] [z opcional]. Tambien podes hacer clic derecho en el mapa.");
            return 1;
        }
        Teletransportar(playerid, floatstr(px), floatstr(py), (pz[0] == EOS) ? 0.0 : floatstr(pz));
        return 1;
    }

    if (!strcmp(comando, "/spawnh", true))
    {
        GuardarSpawn(playerid, BANDO_HUMANO);
        return 1;
    }

    if (!strcmp(comando, "/spawnz", true))
    {
        GuardarSpawn(playerid, BANDO_ZOMBIE);
        return 1;
    }

    if (!strcmp(comando, "/spawns", true))
    {
        MostrarListaSpawns(playerid);
        return 1;
    }

    // /borrarspawn
    new pid[12];
    ObtenerParametro(cmdtext, 1, pid);
    if (pid[0] == EOS)
    {
        SendClientMessage(playerid, COLOR_INFO, "Uso: /borrarspawn [id]. Los ids se ven con /spawns.");
        return 1;
    }
    BorrarSpawn(playerid, strval(pid));
    return 1;
}

// Devuelve 1 si el dialogo era de este modulo
Spawns_AlResponderDialogo(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOGO_SPAWNS) return 0;
    if (!response || !IsPlayerAdmin(playerid)) return 1;
    if (0 <= listitem < gTotalSpawns)
    {
        SetPlayerPos(playerid, gSpawns[listitem][sX], gSpawns[listitem][sY], gSpawns[listitem][sZ]);
        SetPlayerFacingAngle(playerid, gSpawns[listitem][sAngulo]);
    }
    return 1;
}

static GuardarSpawn(playerid, E_BANDO:bando)
{
    if (gTotalSpawns >= MAX_SPAWNS)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Se alcanzo el maximo de spawns. Borra alguno con /borrarspawn.");
        return;
    }

    new Float:x, Float:y, Float:z, Float:angulo, texto[8], consulta[192];
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angulo);
    Bando_Texto(bando, texto);

    mysql_format(gMySQL, consulta, sizeof(consulta),
        "INSERT INTO spawns (bando, pos_x, pos_y, pos_z, angulo) VALUES ('%e', %f, %f, %f, %f)",
        texto, x, y, z, angulo);
    mysql_tquery(gMySQL, consulta, "AlGuardarSpawn", "dddffff",
        playerid, gContadorConsulta[playerid], _:bando, x, y, z, angulo);
}

public AlGuardarSpawn(playerid, contador, bando, Float:x, Float:y, Float:z, Float:angulo)
{
    if (gTotalSpawns >= MAX_SPAWNS) return 1;

    new i = gTotalSpawns++;
    gSpawns[i][sId] = cache_insert_id();
    gSpawns[i][sBando] = E_BANDO:bando;
    gSpawns[i][sX] = x;
    gSpawns[i][sY] = y;
    gSpawns[i][sZ] = z;
    gSpawns[i][sAngulo] = angulo;

    if (contador == gContadorConsulta[playerid])
    {
        new nombre[8], mensaje[96];
        Bando_Nombre(E_BANDO:bando, nombre);
        format(mensaje, sizeof(mensaje), "Spawn %d de %s guardado en %.1f, %.1f, %.1f.", gSpawns[i][sId], nombre, x, y, z);
        SendClientMessage(playerid, COLOR_EXITO, mensaje);
    }
    return 1;
}

static BorrarSpawn(playerid, id)
{
    for (new i = 0; i < gTotalSpawns; i++)
    {
        if (gSpawns[i][sId] != id) continue;

        new consulta[64];
        mysql_format(gMySQL, consulta, sizeof(consulta), "DELETE FROM spawns WHERE id = %d", id);
        mysql_tquery(gMySQL, consulta);

        // Se tapa el hueco con el ultimo de la lista
        gTotalSpawns--;
        if (i != gTotalSpawns) gSpawns[i] = gSpawns[gTotalSpawns];

        new mensaje[48];
        format(mensaje, sizeof(mensaje), "Spawn %d borrado.", id);
        SendClientMessage(playerid, COLOR_EXITO, mensaje);
        return;
    }
    SendClientMessage(playerid, COLOR_ERROR, "No existe un spawn con ese id.");
}

static MostrarListaSpawns(playerid)
{
    if (gTotalSpawns == 0)
    {
        SendClientMessage(playerid, COLOR_INFO, "No hay spawns. Para crear uno: parate en el lugar y usa /spawnh o /spawnz.");
        return;
    }

    new cuerpo[MAX_SPAWNS * 48 + 32] = "ID\tBando\tPosicion\n", linea[48], nombre[8];
    for (new i = 0; i < gTotalSpawns; i++)
    {
        Bando_Nombre(gSpawns[i][sBando], nombre);
        format(linea, sizeof(linea), "%d\t%s\t%.0f, %.0f, %.0f\n",
            gSpawns[i][sId], nombre, gSpawns[i][sX], gSpawns[i][sY], gSpawns[i][sZ]);
        strcat(cuerpo, linea);
    }
    ShowPlayerDialog(playerid, DIALOGO_SPAWNS, DIALOG_STYLE_TABLIST_HEADERS, "Spawns (elegi uno para ir)", cuerpo, "Ir", "Cerrar");
}

// Si z es 0 se busca el suelo; si va en un vehiculo, lo lleva con el
static Teletransportar(playerid, Float:x, Float:y, Float:z)
{
    if (IsPlayerInAnyVehicle(playerid))
    {
        SetVehiclePos(GetPlayerVehicleID(playerid), x, y, z + 1.0);
    }
    else if (z == 0.0)
    {
        SetPlayerPosFindZ(playerid, x, y, 500.0);
    }
    else
    {
        SetPlayerPosFindZ(playerid, x, y, z + 1.0);
    }
}
