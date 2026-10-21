# Amanecer Zombie Reborn

Revival del servidor de supervivencia zombie **Amanecer Zombie** de SA-MP (2016), reconstruido desde cero sobre [open.mp](https://open.mp) con un gamemode en Pawn.

Los zombies son **jugadores**, no NPCs: cada uno juega en el bando Humano o Zombie, y el humano que muere se infecta. El juego transcurre en Los Santos.

## Qué tiene hoy

- **Cuentas en MySQL**: registro y login con SHA-256 + salt, bloqueo por intentos fallidos. Se guardan el dinero, el score, la vida y los ajustes.
- **Vinculación obligatoria con Discord**: después del login, el jugador recibe un código y lo canjea con `/vincular` en el servidor de Discord. Se permite un Discord por cuenta.
- **Baneos permanentes** por IP, serial de la PC (GPCI), nombre y Discord.
- **Bandos** Humano y Zombie, con selector de skin, infección al morir, cura (con el ítem Antídoto o en el skatepark) y el evento **Virus Némesis**.
- **Tiendas**: armas, autos e ítems.
- **Staff** con tres rangos (Moderador, Administrador y CEO). El CEO tiene manos de fuego, regeneración y un RPG.
- **Mapeos del servidor original** (más de 1200 objetos) cargados con el plugin Streamer.
- **Interfaz**: logo, radio, avisos de entrada y salida, y `/config` por jugador (clima, hora, música).

El plan completo, con lo hecho y lo que falta, está en [ROADMAP.md](ROADMAP.md).

## Estructura

```
gamemodes/
  amanecer.pwn        Callbacks de open.mp; delegan en los módulos
  modulos/*.inc       Un archivo por sistema (cuentas, bandos, admin, baneos, discord, ...)
bot/                  Bot de Discord (Node.js) para /vincular
database/schema.sql   Esquema de la base MySQL
config.json           Configuración del servidor open.mp
```

## Requisitos

- Windows
- [open.mp server](https://github.com/openmultiplayer/open.mp/releases) (`omp-server.exe` y la carpeta `components/`)
- Plugins en `plugins/`, cargados desde `legacy_plugins` en `config.json`:
  - [MySQL R41](https://github.com/pBlueG/SA-MP-MySQL/releases)
  - [Streamer v2.9.6](https://github.com/samp-incognito/samp-streamer-plugin/releases), con `streamer.inc` en `qawno/include/`
- MySQL 8
- Node.js 20 o superior (para el bot)
- Cliente SA-MP 0.3.7 para jugar

Los binarios y los plugins no se suben al repositorio: se bajan aparte.

## Instalación

1. **Base de datos.** Crear la base y las tablas:
   ```
   mysql -u root -p < database/schema.sql
   ```
   Después crear el archivo `mysql.ini` en la raíz (no se sube a git):
   ```ini
   hostname = 127.0.0.1
   username = amanecer
   password = tu_clave
   database = amanecer_zombie
   ```
   Las tablas y columnas de los sistemas más nuevos las crea el gamemode al arrancar si no existen.

2. **Compilar el gamemode.** En VS Code se usa la tarea "Compilar gamemode" (Ctrl+Shift+B). Desde la terminal:
   ```
   qawno\pawncc.exe gamemodes\amanecer.pwn -ogamemodes\amanecer.amx -iqawno\include -igamemodes -;+ -(+
   ```

3. **Bot de Discord.**
   - Crear una aplicación con bot en el [Discord Developer Portal](https://discord.com/developers/applications).
   - Invitarlo con los scopes `bot` y `applications.commands` y el permiso **Manage Roles**.
   - Crear el rol que se da al vincular y dejarlo **por debajo** del rol del bot.
   - Copiar `bot/.env.example` como `bot/.env` y completar el token y los IDs. El bot lee MySQL de `mysql.ini`.
   - Arrancarlo:
     ```
     cd bot
     npm install
     npm start
     ```

4. **Servidor.** Ejecutar `omp-server.exe` y conectarse a `127.0.0.1:7777`.

> Mientras el bot esté apagado nadie puede terminar de entrar al juego, porque la vinculación es obligatoria.

## Comandos

**Jugadores**

| Comando | Qué hace |
|---|---|
| `/item` | Ítems: Antídoto, Virus y cambio de skin |
| `/armas` | Tienda de armas (humanos) y motosierra (zombies) |
| `/autos` | Tienda de autos |
| `/id [id]` | Datos de un jugador |
| `/park` | Ir al skatepark |
| `/config` | Clima, hora, logo, radio, avisos y música |
| `/luz` | Ocultar o mostrar los efectos de fuego |
| `/admins` | Staff conectado |

**Staff**

| Comando | Rango mínimo | Qué hace |
|---|---|---|
| `/kick [id] [razón]` | Moderador | Expulsa del juego |
| `/mochila` | Moderador | Equipa un jetpack |
| `/ban [id] [razón]` | Administrador | Ban permanente por IP, PC y Discord |
| `/fly` | Administrador | Modo vuelo |
| `/daradmin`, `/dardinero` | CEO | Dar rango o dinero |
| `/cure [id]`, `/virus [id]` | CEO | Curar o infectar |
| `/spawnh`, `/spawnz`, `/spawns`, `/borrarspawn`, `/ir` | CEO | Armar los spawns de cada bando |
| `/editarfuego`, `/ajustarfuego`, `/poseapuntar`, `/editarnemesis` | CEO | Calibrar los efectos |

Para desbanear a alguien, por ahora hay que poner `activo = 0` en su fila de la tabla `banlist`.

## Cómo se trabaja

- El código, los comentarios y los nombres van en español. Los mensajes que ve el jugador van sin acentos ni ñ, porque el cliente SA-MP los muestra mal.
- Se usa una rama por sistema (`feature/<nombre>`), que se fusiona a `main` cuando funciona.
- Después de cada cambio hay que compilar y verificar que no haya errores ni warnings nuevos.

Más detalle en [CLAUDE.md](CLAUDE.md).
