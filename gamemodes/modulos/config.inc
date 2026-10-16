// ------------------------------------------------------------
//  Configuracion general: constantes compartidas por los modulos
// ------------------------------------------------------------

// Dialogos
#define DIALOGO_LOGIN           1
#define DIALOGO_REGISTRO        2
#define DIALOGO_SPAWNS          3

// Cuentas
#define MAX_INTENTOS_LOGIN      3
#define MIN_LARGO_CLAVE         6
#define LARGO_SALT              16
#define LARGO_HASH              65      // SHA256 en hex (64) + nulo

// Colores de mensajes
#define COLOR_ERROR             0xFF4040FF
#define COLOR_INFO              0xFFFFFFFF
#define COLOR_EXITO             0x00FF00FF

// Colores de bando
#define COLOR_HUMANO            0x00CC00FF
#define COLOR_ZOMBIE            0xFFFFFFFF

// Los mismos colores para insertar dentro de un texto (chat)
#define EMBED_HUMANO            "{00CC00}"
#define EMBED_ZOMBIE            "{FFFFFF}"

// Clima: niebla
#define CLIMA_NIEBLA            9

// Kit de armas de los humanos al spawnear
#define MUNICION_AK47           300
#define MUNICION_MP5            300
#define MUNICION_DEAGLE         70

// Skin automatico de los admins RCON
#define SKIN_ADMIN              155

// Grove Street (Los Santos): lugar del selector de skin y spawn
// de respaldo si un bando no tiene spawns cargados
const Float:SPAWN_INICIAL_X = 2495.0;
const Float:SPAWN_INICIAL_Y = -1687.0;
const Float:SPAWN_INICIAL_Z = 13.5;
const Float:SPAWN_INICIAL_ANGULO = 0.0;

// Camara del login: mira el skatepark de Los Santos (donde estara la cura)
const Float:CAMARA_CIUDAD_X = 1880.0;
const Float:CAMARA_CIUDAD_Y = -1445.0;
const Float:CAMARA_CIUDAD_Z = 35.0;
const Float:CAMARA_MIRA_X = 1920.0;
const Float:CAMARA_MIRA_Y = -1405.0;
const Float:CAMARA_MIRA_Z = 13.5;
