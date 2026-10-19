// ------------------------------------------------------------
//  Configuracion general: constantes compartidas por los modulos
// ------------------------------------------------------------

// Dialogos
#define DIALOGO_LOGIN           1
#define DIALOGO_REGISTRO        2
#define DIALOGO_SPAWNS          3
#define DIALOGO_ITEMS           4
#define DIALOGO_ADMINS          5
#define DIALOGO_CONFIG          6
#define DIALOGO_CONFIG_CLIMA    7
#define DIALOGO_CONFIG_HORA     8
#define DIALOGO_ARMAS           9
#define DIALOGO_PERFIL          10
#define DIALOGO_NEMESIS         11
#define DIALOGO_ARMAS_BLANCAS   12

// Opciones de /config que el jugador puede apagar (bits de users.cfg_flags).
// Se guardan las desactivadas: 0 = todo activado.
#define CFG_SIN_LOGO            1
#define CFG_SIN_RADIO           2
#define CFG_SIN_AVISOS          4
#define CFG_SIN_MUSICA          8

// Rangos de admin (users.admin_nivel). El CEO solo se asigna en la base.
#define RANGO_NINGUNO           0
#define RANGO_MODERADOR         1
#define RANGO_ADMINISTRADOR     2
#define RANGO_CEO               3
#define EMBED_MODERADOR         "{00BFFF}"      // celeste
#define EMBED_ADMINISTRADOR     "{FF2020}"      // rojo
#define EMBED_CEO               "{FFD700}"      // dorado

// Precios de /item
#define PRECIO_ANTIDOTO         150000
#define PRECIO_VIRUS            20000

// Cuentas
#define MAX_INTENTOS_LOGIN      3
#define MIN_LARGO_CLAVE         6
#define LARGO_SALT              16
#define LARGO_HASH              65      // SHA256 en hex (64) + nulo

// Colores de mensajes
#define COLOR_ERROR             0xFF4040FF
#define COLOR_INFO              0xFFFFFFFF
#define COLOR_EXITO             0x00FF00FF
#define COLOR_AVISO             0x808080FF      // entradas y salidas de jugadores (gris oscuro)
#define COLOR_INFECTADO         0xB00000FF      // mensajes propios del zombie (rojo oscuro)

// Colores de bando
#define COLOR_HUMANO            0x00CC00FF
#define COLOR_ZOMBIE            0xFFFFFFFF

// Los mismos colores para insertar dentro de un texto (chat)
#define EMBED_HUMANO            "{00CC00}"
#define EMBED_ZOMBIE            "{FFFFFF}"
#define EMBED_NEMESIS           "{FF0000}"

// Musica del selector de skin (MP3 con link directo; el cliente 0.3.7 prefiere http)
#define MUSICA_SELECTOR         "https://files.catbox.moe/fpbvi0.mp3"

// Clima y hora del servidor (cada jugador puede cambiarlos para si con /config)
#define CLIMA_NIEBLA            9
#define HORA_SERVIDOR           12

// Kit de armas de los humanos al spawnear
#define MUNICION_AK47           300
#define MUNICION_MP5            300
#define MUNICION_DEAGLE         70

// Skin automatico del CEO
#define SKIN_ADMIN              155
#define MUNICION_RPG_CEO        10      // el CEO spawnea con un RPG

// Skin exclusivo del Nemesis: no se puede elegir en ningun selector
#define SKIN_NEMESIS            17

// Particula que aparece donde muere un jugador
#define MODELO_EFECTO_MUERTE    18668
#define DURACION_EFECTO_MUERTE  2000    // ms hasta que desaparece

// Manos de fuego del CEO
#define MODELO_FUEGO            18693   // particulas de fuego
#define SLOT_FUEGO_IZQ          8       // slots de objetos pegados (0-9)
#define SLOT_FUEGO_DER          9
#define HUESO_MANO_IZQ          5
#define HUESO_MANO_DER          6
#define REGEN_INTERVALO         2000    // ms
const Float:REGEN_CANTIDAD = 10.0;      // vida y chaleco por intervalo

// Grove Street (Los Santos): spawn
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
