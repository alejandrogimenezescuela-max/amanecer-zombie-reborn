// ------------------------------------------------------------
//  Cuentas: registro, login y guardado en MySQL
// ------------------------------------------------------------

forward AlCargarCuenta(playerid, contador);
forward AlRegistrarCuenta(playerid, contador);
forward MostrarCiudad(playerid, contador);
forward KickDiferido(playerid, contador);

// IPs que agotaron los intentos de login: no pueden volver a probar por un rato.
// Asi reconectar no sirve para adivinar claves sin limite.
#define MAX_IPS_BLOQUEADAS      32
#define BLOQUEO_LOGIN_MS        (2 * 60 * 1000)     // 2 minutos

static gIpBloqueada[MAX_IPS_BLOQUEADAS][16];
static gBloqueoHasta[MAX_IPS_BLOQUEADAS];

static BloquearIp(playerid)
{
    new ip[16], ahora = GetTickCount(), libre = 0;
    GetPlayerIp(playerid, ip, sizeof(ip));

    // Se reusa la misma IP, un lugar vencido o, si no hay, el que vence antes
    for (new i = 0; i < MAX_IPS_BLOQUEADAS; i++)
    {
        if (!strcmp(gIpBloqueada[i], ip) || gBloqueoHasta[i] - ahora <= 0) { libre = i; break; }
        if (gBloqueoHasta[i] - gBloqueoHasta[libre] < 0) libre = i;
    }
    strcopy(gIpBloqueada[libre], ip, sizeof(gIpBloqueada[]));
    gBloqueoHasta[libre] = ahora + BLOQUEO_LOGIN_MS;
}

// Segundos que le faltan a la IP del jugador para poder volver a entrar (0 = libre)
static SegundosDeBloqueo(playerid)
{
    new ip[16], ahora = GetTickCount();
    GetPlayerIp(playerid, ip, sizeof(ip));

    for (new i = 0; i < MAX_IPS_BLOQUEADAS; i++)
    {
        new restante = gBloqueoHasta[i] - ahora;
        if (restante > 0 && gIpBloqueada[i][0] != EOS && !strcmp(gIpBloqueada[i], ip)) return restante / 1000 + 1;
    }
    return 0;
}

// Busca la cuenta por nombre al conectar
Cuentas_AlConectar(playerid)
{
    PonerEnEspera(playerid);

    new espera = SegundosDeBloqueo(playerid);
    if (espera > 0)
    {
        new mensaje[96];
        format(mensaje, sizeof(mensaje), "Demasiados intentos fallidos. Espera %d segundos para volver a entrar.", espera);
        SendClientMessage(playerid, COLOR_ERROR, mensaje);
        ExpulsarConRetraso(playerid);
        return;
    }

    new nombre[MAX_PLAYER_NAME + 1], consulta[160];
    GetPlayerName(playerid, nombre, sizeof(nombre));
    mysql_format(gMySQL, consulta, sizeof(consulta),
        "SELECT id, password_hash, salt, vida, dinero, score, admin_nivel, vip_nivel, cfg_clima, cfg_hora, cfg_flags FROM users WHERE nombre = '%e' LIMIT 1",
        nombre);
    mysql_tquery(gMySQL, consulta, "AlCargarCuenta", "dd", playerid, gContadorConsulta[playerid]);
}

public AlCargarCuenta(playerid, contador)
{
    if (contador != gContadorConsulta[playerid]) return 1;

    if (cache_num_rows() > 0)
    {
        // El bando y el skin no se cargan: se eligen de nuevo en cada sesion
        cache_get_value_name_int(0, "id", gJugador[playerid][jId]);
        cache_get_value_name(0, "password_hash", gJugador[playerid][jHash], LARGO_HASH);
        cache_get_value_name(0, "salt", gJugador[playerid][jSalt], LARGO_SALT + 1);
        cache_get_value_name_float(0, "vida", gJugador[playerid][jVida]);
        cache_get_value_name_int(0, "dinero", gJugador[playerid][jDinero]);
        cache_get_value_name_int(0, "admin_nivel", gJugador[playerid][jAdminNivel]);
        cache_get_value_name_int(0, "vip_nivel", gJugador[playerid][jVipNivel]);
        cache_get_value_name_int(0, "score", gJugador[playerid][jScore]);
        cache_get_value_name_int(0, "cfg_clima", gJugador[playerid][jCfgClima]);
        cache_get_value_name_int(0, "cfg_hora", gJugador[playerid][jCfgHora]);
        cache_get_value_name_int(0, "cfg_flags", gJugador[playerid][jCfgFlags]);
        if (gJugador[playerid][jVida] <= 0.0) gJugador[playerid][jVida] = 100.0;

        MostrarLogin(playerid, "");
    }
    else
    {
        MostrarRegistro(playerid, "");
    }

    // La musica suena desde el login hasta que spawnea (con los ajustes ya cargados)
    Bando_IniciarMusica(playerid);
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

// Devuelve 1 si el dialogo era de este modulo
Cuentas_AlResponderDialogo(playerid, dialogid, response, const inputtext[])
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
                    BloquearIp(playerid);
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

            // El skin y el bando se eligen en el selector y se guardan al salir
            new nombre[MAX_PLAYER_NAME + 1], ip[16], consulta[384];
            GetPlayerName(playerid, nombre, sizeof(nombre));
            GetPlayerIp(playerid, ip, sizeof(ip));
            mysql_format(gMySQL, consulta, sizeof(consulta),
                "INSERT INTO users (nombre, password_hash, salt, ultima_ip, ultimo_ingreso) VALUES ('%e', '%e', '%e', '%e', NOW())",
                nombre, gJugador[playerid][jHash], gJugador[playerid][jSalt], ip);
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

// Sale del modo espectador directo a la seleccion de skin
IniciarSesion(playerid)
{
    gJugador[playerid][jLogueado] = true;
    SetPlayerScore(playerid, gJugador[playerid][jScore]);
    SendClientMessage(playerid, COLOR_INFO, "Elegi tu skin con las flechas. Los skins de zombie te ponen en el bando Zombie.");
    ForceClassSelection(playerid);
    TogglePlayerSpectating(playerid, false);
}

// Al aparecer: en un spawn al azar de su bando, con la vida guardada
Cuentas_AlSpawnear(playerid)
{
    new Float:x, Float:y, Float:z, Float:angulo;
    Spawns_Elegir(gJugador[playerid][jBando], x, y, z, angulo);

    gJugador[playerid][jEnJuego] = true;
    gJugador[playerid][jMuerto] = false;

    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, angulo);
    SetPlayerHealth(playerid, gJugador[playerid][jVida]);
    SetCameraBehindPlayer(playerid);
    Dinero_Sincronizar(playerid);
}

// Al morir reaparece con vida llena y el mismo skin (la clase elegida se mantiene)
Cuentas_AlMorir(playerid)
{
    gJugador[playerid][jMuerto] = true;
    gJugador[playerid][jVida] = 100.0;
}

// ------------------------------------------------------------
//  Guardado
// ------------------------------------------------------------

GuardarCuenta(playerid)
{
    // Si todavia esta en el selector no hay nada nuevo que guardar
    if (!gJugador[playerid][jLogueado] || !gJugador[playerid][jEnJuego]) return 0;

    new consulta[256], bando[8], Float:vida;
    Bando_Texto(gJugador[playerid][jBando], bando);
    GetPlayerHealth(playerid, vida);

    // Si murio, la proxima vez entra con vida llena. Nunca mas de 100
    // (mientras vuela con /fly la vida es infinita).
    if (gJugador[playerid][jMuerto] || vida <= 0.0 || vida > 100.0) vida = 100.0;

    mysql_format(gMySQL, consulta, sizeof(consulta),
        "UPDATE users SET bando = '%e', skin = %d, vida = %f, dinero = %d, \
         score = %d, cfg_clima = %d, cfg_hora = %d, cfg_flags = %d WHERE id = %d",
        bando, gJugador[playerid][jSkin], vida, gJugador[playerid][jDinero],
        GetPlayerScore(playerid), gJugador[playerid][jCfgClima], gJugador[playerid][jCfgHora], gJugador[playerid][jCfgFlags],
        gJugador[playerid][jId]);
    mysql_tquery(gMySQL, consulta);
    return 1;
}

// Al cerrar el servidor: guarda a todos antes de cerrar MySQL
Cuentas_GuardarTodos()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i))
        {
            GuardarCuenta(i);
            gJugador[i][jLogueado] = false;
        }
    }
}

// ------------------------------------------------------------
//  Espera, salt y kick
// ------------------------------------------------------------

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
