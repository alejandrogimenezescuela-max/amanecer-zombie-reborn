#include <open.mp>
#include <a_mysql>

// ------------------------------------------------------------
//  Constantes
// ------------------------------------------------------------

#define DIALOGO_LOGIN           1
#define DIALOGO_REGISTRO        2

#define MAX_INTENTOS_LOGIN      3
#define MIN_LARGO_CLAVE         6
#define LARGO_SALT              16
#define LARGO_HASH              65      // SHA256 en hex (64) + nulo

#define COLOR_ERROR             0xFF4040FF
#define COLOR_INFO              0xFFFFFFFF
#define COLOR_EXITO             0x00FF00FF

// Spawn inicial: Grove Street (Los Santos)
const Float:SPAWN_INICIAL_X = 2495.0;
const Float:SPAWN_INICIAL_Y = -1687.0;
const Float:SPAWN_INICIAL_Z = 13.5;
const Float:SPAWN_INICIAL_ANGULO = 0.0;

// Camara que mira la ciudad mientras el jugador no inicia sesion
const Float:CAMARA_CIUDAD_X = 1414.0;
const Float:CAMARA_CIUDAD_Y = -868.0;
const Float:CAMARA_CIUDAD_Z = 120.0;
const Float:CAMARA_MIRA_X = 1530.0;
const Float:CAMARA_MIRA_Y = -1350.0;
const Float:CAMARA_MIRA_Z = 60.0;

// Skins de civil para cuentas nuevas
static const gSkinsCiviles[] =
{
    1, 2, 7, 14, 15, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30,
    35, 36, 37, 44, 46, 47, 48, 58, 59, 60, 101, 170, 188, 250
};

// ------------------------------------------------------------
//  Datos de jugador
// ------------------------------------------------------------

enum E_JUGADOR
{
    jId,
    jHash[LARGO_HASH],
    jSalt[LARGO_SALT + 1],
    bool:jLogueado,
    jIntentos,
    jSkin,
    Float:jVida,
    Float:jX,
    Float:jY,
    Float:jZ,
    Float:jAngulo,
    bool:jMuerto
}

new MySQL:gMySQL;
new gJugador[MAX_PLAYERS][E_JUGADOR];

// Se incrementa al conectar y desconectar; si una respuesta de MySQL llega
// con un valor viejo, el jugador original ya se fue y se descarta.
new gContadorConsulta[MAX_PLAYERS];

forward AlCargarCuenta(playerid, contador);
forward AlRegistrarCuenta(playerid, contador);
forward MostrarCiudad(playerid, contador);
forward KickDiferido(playerid, contador);

main()
{
    print("Amanecer Zombie Reborn - gamemode cargado");
}

public OnGameModeInit()
{
    // Registra todo lo que pasa con MySQL en logs/plugins/mysql.log
    mysql_log(ALL);

    gMySQL = mysql_connect_file();
    if (gMySQL == MYSQL_INVALID_HANDLE || mysql_errno(gMySQL) != 0)
    {
        printf("[MySQL] ERROR %d: no se pudo conectar. Revisa mysql.ini", mysql_errno(gMySQL));
    }
    else
    {
        print("[MySQL] Conectado a amanecer_zombie.");
    }

    SetGameModeText("Zombie Survival");

    // Clase por defecto; el spawn real lo define SetSpawnInfo al iniciar sesion
    AddPlayerClass(0, SPAWN_INICIAL_X, SPAWN_INICIAL_Y, SPAWN_INICIAL_Z, SPAWN_INICIAL_ANGULO, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    return 1;
}

public OnGameModeExit()
{
    // Guarda a todos antes de cerrar la conexion (mysql_close espera las consultas pendientes)
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i))
        {
            GuardarCuenta(i);
            gJugador[i][jLogueado] = false;
        }
    }

    mysql_close(gMySQL);
    return 1;
}

// ------------------------------------------------------------
//  Conexion y carga de cuenta
// ------------------------------------------------------------

public OnPlayerConnect(playerid)
{
    ReiniciarDatos(playerid);
    gContadorConsulta[playerid]++;

    SendClientMessage(playerid, COLOR_EXITO, "Bienvenido a Amanecer Zombie Reborn.");
    SendClientMessage(playerid, COLOR_INFO, "El mundo cayo. Sobrevivi como puedas...");

    PonerEnEspera(playerid);

    new nombre[MAX_PLAYER_NAME + 1], consulta[128];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    mysql_format(gMySQL, consulta, sizeof(consulta),
        "SELECT id, password_hash, salt, skin, vida, pos_x, pos_y, pos_z, angulo FROM users WHERE nombre = '%e' LIMIT 1",
        nombre);
    mysql_tquery(gMySQL, consulta, "AlCargarCuenta", "dd", playerid, gContadorConsulta[playerid]);
    return 1;
}

public AlCargarCuenta(playerid, contador)
{
    if (contador != gContadorConsulta[playerid]) return 1;

    if (cache_num_rows() > 0)
    {
        new bool:sinPosicion;
        cache_get_value_name_int(0, "id", gJugador[playerid][jId]);
        cache_get_value_name(0, "password_hash", gJugador[playerid][jHash], LARGO_HASH);
        cache_get_value_name(0, "salt", gJugador[playerid][jSalt], LARGO_SALT + 1);
        cache_get_value_name_int(0, "skin", gJugador[playerid][jSkin]);
        cache_get_value_name_float(0, "vida", gJugador[playerid][jVida]);

        // pos_x en NULL = cuenta sin posicion guardada o murio antes de salir
        cache_is_value_name_null(0, "pos_x", sinPosicion);
        if (sinPosicion)
        {
            PosicionInicial(playerid);
        }
        else
        {
            cache_get_value_name_float(0, "pos_x", gJugador[playerid][jX]);
            cache_get_value_name_float(0, "pos_y", gJugador[playerid][jY]);
            cache_get_value_name_float(0, "pos_z", gJugador[playerid][jZ]);
            cache_get_value_name_float(0, "angulo", gJugador[playerid][jAngulo]);
        }

        MostrarLogin(playerid, "");
    }
    else
    {
        MostrarRegistro(playerid, "");
    }
    return 1;
}

// ------------------------------------------------------------
//  Dialogos
// ------------------------------------------------------------

MostrarLogin(playerid, const aviso[])
{
    new cuerpo[192], nombre[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(cuerpo, sizeof(cuerpo), "%s{FFFFFF}Bienvenido de nuevo, %s.\nIngresa tu clave para iniciar sesion.\n\nIntentos restantes: %d",
        aviso, nombre, MAX_INTENTOS_LOGIN - gJugador[playerid][jIntentos]);
    ShowPlayerDialog(playerid, DIALOGO_LOGIN, DIALOG_STYLE_PASSWORD, "Iniciar sesion", cuerpo, "Entrar", "Salir");
}

MostrarRegistro(playerid, const aviso[])
{
    new cuerpo[192], nombre[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    format(cuerpo, sizeof(cuerpo), "%s{FFFFFF}La cuenta %s no esta registrada.\nElegi una clave de al menos %d caracteres.",
        aviso, nombre, MIN_LARGO_CLAVE);
    ShowPlayerDialog(playerid, DIALOGO_REGISTRO, DIALOG_STYLE_PASSWORD, "Registro", cuerpo, "Registrar", "Salir");
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch (dialogid)
    {
        case DIALOGO_LOGIN:
        {
            // Solo vale si hay una cuenta cargada y todavia no inicio sesion
            if (gJugador[playerid][jLogueado] || gJugador[playerid][jId] == 0) return 1;

            if (!response)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Saliste del servidor.");
                ExpulsarConRetraso(playerid);
                return 1;
            }

            new hash[LARGO_HASH];
            SHA256_Hash(inputtext, gJugador[playerid][jSalt], hash, sizeof(hash));

            if (strcmp(hash, gJugador[playerid][jHash]) != 0)
            {
                gJugador[playerid][jIntentos]++;
                if (gJugador[playerid][jIntentos] >= MAX_INTENTOS_LOGIN)
                {
                    SendClientMessage(playerid, COLOR_ERROR, "Clave incorrecta. Agotaste los intentos y fuiste expulsado.");
                    ExpulsarConRetraso(playerid);
                }
                else
                {
                    MostrarLogin(playerid, "{FF4040}Clave incorrecta.\n\n");
                }
                return 1;
            }

            new ip[16], consulta[128];
            GetPlayerIp(playerid, ip, sizeof(ip));
            mysql_format(gMySQL, consulta, sizeof(consulta),
                "UPDATE users SET ultimo_ingreso = NOW(), ultima_ip = '%e' WHERE id = %d",
                ip, gJugador[playerid][jId]);
            mysql_tquery(gMySQL, consulta);

            SendClientMessage(playerid, COLOR_EXITO, "Iniciaste sesion correctamente.");
            IniciarSesion(playerid);
            return 1;
        }
        case DIALOGO_REGISTRO:
        {
            if (gJugador[playerid][jLogueado] || gJugador[playerid][jId] != 0) return 1;

            if (!response)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Saliste del servidor.");
                ExpulsarConRetraso(playerid);
                return 1;
            }

            if (strlen(inputtext) < MIN_LARGO_CLAVE)
            {
                MostrarRegistro(playerid, "{FF4040}La clave es demasiado corta.\n\n");
                return 1;
            }

            GenerarSalt(gJugador[playerid][jSalt], LARGO_SALT + 1);
            SHA256_Hash(inputtext, gJugador[playerid][jSalt], gJugador[playerid][jHash], LARGO_HASH);

            gJugador[playerid][jSkin] = gSkinsCiviles[random(sizeof(gSkinsCiviles))];
            PosicionInicial(playerid);

            new nombre[MAX_PLAYER_NAME + 1], ip[16], consulta[384];
            GetPlayerName(playerid, nombre, sizeof(nombre));
            GetPlayerIp(playerid, ip, sizeof(ip));
            mysql_format(gMySQL, consulta, sizeof(consulta),
                "INSERT INTO users (nombre, password_hash, salt, skin, ultima_ip, ultimo_ingreso) VALUES ('%e', '%e', '%e', %d, '%e', NOW())",
                nombre, gJugador[playerid][jHash], gJugador[playerid][jSalt], gJugador[playerid][jSkin], ip);
            mysql_tquery(gMySQL, consulta, "AlRegistrarCuenta", "dd", playerid, gContadorConsulta[playerid]);
            return 1;
        }
    }
    return 0;
}

public AlRegistrarCuenta(playerid, contador)
{
    if (contador != gContadorConsulta[playerid]) return 1;

    gJugador[playerid][jId] = cache_insert_id();
    SendClientMessage(playerid, COLOR_EXITO, "Cuenta registrada. Bienvenido al apocalipsis.");
    IniciarSesion(playerid);
    return 1;
}

// ------------------------------------------------------------
//  Sesion y spawn
// ------------------------------------------------------------

IniciarSesion(playerid)
{
    gJugador[playerid][jLogueado] = true;

    SetSpawnInfo(playerid, NO_TEAM, gJugador[playerid][jSkin],
        gJugador[playerid][jX], gJugador[playerid][jY], gJugador[playerid][jZ], gJugador[playerid][jAngulo]);

    // Al salir del modo espectador el jugador aparece con SetSpawnInfo
    TogglePlayerSpectating(playerid, false);
}

public OnPlayerRequestClass(playerid, classid)
{
    if (!gJugador[playerid][jLogueado])
    {
        PonerEnEspera(playerid);
    }
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    // Nadie spawnea sin iniciar sesion
    if (!gJugador[playerid][jLogueado]) return 0;
    return 1;
}

public OnPlayerSpawn(playerid)
{
    if (!gJugador[playerid][jLogueado])
    {
        PonerEnEspera(playerid);
        return 1;
    }

    gJugador[playerid][jMuerto] = false;
    SetPlayerHealth(playerid, gJugador[playerid][jVida]);
    SendClientMessage(playerid, COLOR_ERROR, "Cuidado: los zombies estan cerca.");
    return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    if (!gJugador[playerid][jLogueado]) return 1;

    // Al morir vuelve al spawn inicial con vida llena
    gJugador[playerid][jMuerto] = true;
    gJugador[playerid][jSkin] = GetPlayerSkin(playerid);
    PosicionInicial(playerid);
    SetSpawnInfo(playerid, NO_TEAM, gJugador[playerid][jSkin],
        gJugador[playerid][jX], gJugador[playerid][jY], gJugador[playerid][jZ], gJugador[playerid][jAngulo]);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    GuardarCuenta(playerid);
    gContadorConsulta[playerid]++;
    ReiniciarDatos(playerid);
    return 1;
}

// ------------------------------------------------------------
//  Guardado
// ------------------------------------------------------------

GuardarCuenta(playerid)
{
    if (!gJugador[playerid][jLogueado]) return 0;

    new consulta[256], Float:vida;
    GetPlayerHealth(playerid, vida);

    if (gJugador[playerid][jMuerto] || vida <= 0.0)
    {
        // Murio: la proxima vez aparece en el spawn inicial con vida llena
        mysql_format(gMySQL, consulta, sizeof(consulta),
            "UPDATE users SET skin = %d, vida = 100.0, pos_x = NULL, pos_y = NULL, pos_z = NULL, angulo = NULL WHERE id = %d",
            gJugador[playerid][jSkin], gJugador[playerid][jId]);
    }
    else
    {
        new Float:x, Float:y, Float:z, Float:angulo;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, angulo);
        mysql_format(gMySQL, consulta, sizeof(consulta),
            "UPDATE users SET skin = %d, vida = %f, pos_x = %f, pos_y = %f, pos_z = %f, angulo = %f WHERE id = %d",
            GetPlayerSkin(playerid), vida, x, y, z, angulo, gJugador[playerid][jId]);
    }
    mysql_tquery(gMySQL, consulta);
    return 1;
}

// ------------------------------------------------------------
//  Utilidades
// ------------------------------------------------------------

ReiniciarDatos(playerid)
{
    gJugador[playerid][jId] = 0;
    gJugador[playerid][jHash][0] = EOS;
    gJugador[playerid][jSalt][0] = EOS;
    gJugador[playerid][jLogueado] = false;
    gJugador[playerid][jIntentos] = 0;
    gJugador[playerid][jSkin] = 0;
    gJugador[playerid][jMuerto] = false;
    PosicionInicial(playerid);
}

// Spawn de Grove Street con vida llena
PosicionInicial(playerid)
{
    gJugador[playerid][jX] = SPAWN_INICIAL_X;
    gJugador[playerid][jY] = SPAWN_INICIAL_Y;
    gJugador[playerid][jZ] = SPAWN_INICIAL_Z;
    gJugador[playerid][jAngulo] = SPAWN_INICIAL_ANGULO;
    gJugador[playerid][jVida] = 100.0;
}

// Modo espectador mirando la ciudad hasta que inicie sesion
PonerEnEspera(playerid)
{
    TogglePlayerSpectating(playerid, true);
    // La camara se fija un instante despues para que el cliente ya este en modo espectador
    SetTimerEx("MostrarCiudad", 250, false, "dd", playerid, gContadorConsulta[playerid]);
}

public MostrarCiudad(playerid, contador)
{
    if (contador != gContadorConsulta[playerid] || gJugador[playerid][jLogueado]) return 1;

    SetPlayerCameraPos(playerid, CAMARA_CIUDAD_X, CAMARA_CIUDAD_Y, CAMARA_CIUDAD_Z);
    SetPlayerCameraLookAt(playerid, CAMARA_MIRA_X, CAMARA_MIRA_Y, CAMARA_MIRA_Z);
    return 1;
}

GenerarSalt(salt[], largo)
{
    static const caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (new i = 0; i < largo - 1; i++)
    {
        salt[i] = caracteres[random(sizeof(caracteres) - 1)];
    }
    salt[largo - 1] = EOS;
}

// Kick con un pequeno retraso para que el jugador llegue a leer el mensaje
ExpulsarConRetraso(playerid)
{
    SetTimerEx("KickDiferido", 500, false, "dd", playerid, gContadorConsulta[playerid]);
}

public KickDiferido(playerid, contador)
{
    if (contador == gContadorConsulta[playerid] && IsPlayerConnected(playerid)) Kick(playerid);
    return 1;
}
