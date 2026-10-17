# Amanecer Zombie Reborn

Recreación de un servidor de supervivencia zombie de SA-MP, construido sobre **open.mp** con gamemode en **Pawn**. Se desarrolla paso a paso, un sistema por vez.

## Entorno

- Windows, VS Code.
- Servidor: `omp-server.exe` (open.mp), configuración en `config.json`.
- Gamemode principal: `gamemodes/amanecer.pwn` → compila a `gamemodes/amanecer.amx`.
- Compilador: `qawno/pawncc.exe` con includes en `qawno/include` (`#include <open.mp>`).
- Compilar (tarea de VS Code "Compilar gamemode", Ctrl+Shift+B), o desde la terminal:
  ```
  qawno\pawncc.exe gamemodes\amanecer.pwn -ogamemodes\amanecer.amx -iqawno\include -igamemodes -;+ -(+
  ```
- El gamemode está dividido en módulos en `gamemodes/modulos/*.inc` (config, utiles, jugador, mysql, dinero, bandos, spawns, cuentas, admin, conexiones, items, efectos, vuelo, interfaz, parque, ajustes). `amanecer.pwn` solo tiene los callbacks y delega en funciones de cada módulo (sin y_hooks).
- Base de datos: **MySQL** (según `ROADMAP.md`). El primer borrador del sistema de cuentas usó SQLite (`scriptfiles/amanecer.db`) y hay que migrarlo.
- Para probar: cliente SA-MP 0.3.7 conectando a `127.0.0.1:7777`.

## Convenciones

- Código, comentarios y nombres de variables/funciones en español.
- Mensajes que ve el jugador **sin acentos ni ñ** (el cliente SA-MP los muestra mal).
- Usar las funciones nuevas de open.mp (`DB_Open`, `DB_ExecuteQuery`, etc.), no las viejas deprecadas (`db_open`, `db_query`).
- Usar `%q` en `format` para cualquier texto del jugador que vaya a una consulta SQL.
- Después de cada cambio, compilar y verificar que no haya errores ni warnings nuevos.

## Git

- Repositorio en GitHub: `alejandrogimenezescuela-max/amanecer-zombie-reborn`.
- Una rama por sistema: `feature/<nombre>` (ej. `feature/cuentas`), se fusiona a `main` cuando funciona.
- No se suben binarios ni archivos generados (`*.exe`, `*.dll`, `components/`, `plugins/`, `*.amx`, logs, `*.db`); ver `.gitignore`.

## Hoja de ruta

El plan oficial está en **`ROADMAP.md`** (20 días, 4 fases). Seguirlo en orden y marcar las tareas completadas.

Puntos clave del diseño:
- Los zombies son **jugadores** (bando Humano / Zombie), no NPCs.
- Base de datos de producción: **MySQL** (tablas `users`, `banlist`, `clans`).
- Juego centrado en **Los Santos**; San Fierro y Las Venturas quedan limpios.
- Eventos: Royal Rumble, Némesis, Masacre. VIP con "Manos de Fuego". Clanes.
- Anticheat: NEX-AC + Pawn.RakNet; identificación por GPCI, IP/subred y Discord.

Los detalles de cómo era el servidor original los va contando el dueño del proyecto a medida que avanzamos; preguntar antes de inventar mecánicas.
