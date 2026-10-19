# 📅 Roadmap de 20 Días: Del Código al Lanzamiento

**Amanecer Zombie Reborn** — revival del servidor de 2016 sobre open.mp.

Marcá cada tarea con `[x]` a medida que la completes.

---

## FASE 1: Cimientos Técnicos y Nostalgia (Días 1 al 5)

### Día 1: Entorno de Desarrollo y Persistencia
- [x] Configurar open.mp local en la PC
- [x] Repositorio en GitHub y entorno en VS Code
- [x] Crear la base de datos MySQL con las tablas:
  - [x] `users`
  - [x] `banlist`
  - [x] `clans`
- [x] Sistema de cuentas: registro/login con MySQL y guardado de skin, bando y vida

### Día 2: Sistema de Spawn y Clases
- [x] Gamemode dividido en módulos (`gamemodes/modulos/*.inc`)
- [x] Selección de bando: **Humano / Zombie**
  - [x] Selector de skin con texto HUMANO (verde) / ZOMBIE (blanco); skins de zombie agrupados
  - [x] El humano que muere se infecta y reaparece como zombie, con aviso a todo el servidor
  - [x] Nombre en el chat con el color del bando
- [ ] Habilidades iniciales de cada bando
  - [x] Humanos: kit de armas (AK-47, MP5, Desert Eagle) y frases al spawnear
  - [ ] Zombies: por definir
- [x] Spawns aleatorios por bando (tabla `spawns`) y herramientas de admin para armarlos (`/spawnh`, `/spawnz`, `/spawns`, `/borrarspawn`, `/ir`, clic en el mapa)
- [ ] Punto de spawn neurálgico en Los Santos (Safezone principal / Hospital)
- [x] Clima de niebla, animaciones de CJ para todos los skins
- [x] Avisos `[<!>]` de entrada/salida (con razón), infección y cura
- [x] Partícula al morir
- [x] `/item`: Antídoto ($150.000), Virus ($20.000) y selector de skin con imágenes
- [x] Dinero guardado en `users.dinero` (por ahora solo se consigue con `/dardinero`)
- [x] Rangos de admin en `users.admin_nivel`: Moderador, Administrador y CEO; `/admin` y `/daradmin`
- [x] CEO: skin 155, RPG con 10 cohetes, manos de fuego con regeneración, `/cure`, `/virus`, `/dardinero` y herramientas de spawns
- [x] Manos de fuego con un ajuste por postura (manos sueltas, sosteniendo y apuntando/disparando con cada arma), guardado en la tabla `fuego_ajustes`; las armas sin calibrar usan el ajuste del AK-47. Calibración: `/editarfuego`, `/ajustarfuego`, `/poseapuntar`
- [ ] Manos de fuego con el skin 230: revisar (puede necesitar un ajuste por skin)
- [x] RCON sin poderes dentro del juego (contraseña aleatoria)
- [x] `/fly` para Administrador y CEO (vida infinita mientras vuela)
- [x] Selector de skin en el lobby de Zombotech (San Fierro); música de fondo desde el login hasta spawnear
- [x] Logo "Amanecer Zombie Reborn" arriba del minimapa (textdraws)
- [x] Radio transmisor cada 5 minutos y `/park` para ir al skatepark
- [x] `/config`: clima, hora, logo, radio, avisos y música por jugador (guardado en la cuenta)
- [x] `/armas`: tienda de armas para humanos (+500 balas por compra, se suman) y motosierra para zombies
- [x] `/autos`: 15 autos comunes con vista previa en colores, nombre y precio (un auto por jugador)
- [x] `/id`: datos de un jugador (bando, rango, VIP, dinero y score); score guardado en la cuenta
- [x] Cambio de skin con `/item` con espera de 3 minutos
- [x] Chat y frases con el nick del color del bando (verde humano, blanco zombie, rojo Nemesis) y texto en blanco
- [x] **Virus Nemesis**: pickup que se compra con 5000 de score; zombie con skin exclusivo 17, RPG infinito, 5000 de vida virtual, 100 de chaleco y efecto propio (`/editarnemesis`)
- [x] `/luz` para ocultar o mostrar las manos de fuego y el efecto del Nemesis
- [x] Mapeos rescatados del servidor original (1140 objetos) con el plugin Streamer
- [x] Precios de /armas copiados del servidor original (AK-47 $8.000 y motosierra $3.000 a criterio) y precios de autos confirmados
- [x] Cura frente al skatepark: checkpoint que cura a los zombies en 5 s ("APLICANDO ANTIDOTO") y cruz roja en el radar; transmision militar cada 5 minutos
- [ ] Mapear la zona de la cura
- [ ] Definir cómo se gana dinero jugando
- [ ] Definir qué comandos tienen Moderador y Administrador

### Día 3: Sistema VIP & Balance
- [ ] Regeneración pasiva lenta (**Manos de Fuego**)
- [ ] Kits de armas limitadas (RPG / Uzi / Sawnoff) con cooldowns estrictos

### Días 4 y 5: Mapeo y Puntos de Loot
- [ ] Mapear zonas de supervivencia en Los Santos
- [ ] Cofres/cajas de suministros (**Loot Zones**)
- [ ] Dejar San Fierro y Las Venturas limpios

---

## FASE 2: Eventos y Seguridad Multi-Capa (Días 6 al 10)

### Día 6: Evento Royal Rumble
- [ ] Comando `/evens`
- [ ] Teletransporte a la cúpula de la grúa
- [ ] Entrega de bates/dildos
- [ ] Detección de **Último en Pie**

### Día 7: Eventos Némesis y Masacre
- [ ] **Némesis:** skin especial, minigun/RPG y dimensión virtual aislada
- [ ] **Masacre:** área delimitada para combate FFA con traición final

### Día 8: Sistema de Clanes
- [ ] Comando `/clan`
- [ ] Chat privado de clan
- [ ] Tag en el scoreboard
- [ ] 2 slots oficiales iniciales con base fortificada

### Día 9: Seguridad Server-Side (Anti-Cheat Base)
- [ ] Integración del módulo **NEX-AC**
- [ ] Filtros con **Pawn.RakNet** contra:
  - [ ] Airbreak
  - [ ] TP Hack
  - [ ] Speedhack
  - [ ] Weapon Spawner

### Día 10: Sistema de Identificación (GPCI + Banlist)
- [ ] Capturar serial GPCI (`gpci()`), IP y subred en MySQL al conectar (`OnPlayerConnect`)
- [ ] Alertas silenciosas en pantalla para admins
- [ ] Comando `/spec ID`

---

## FASE 3: Comunidad, Marketing y Discord OAuth2 (Días 11 al 15)

### Día 11: Servidor de Discord Oficial
- [ ] Categorías y canales
- [ ] Roles: **Maestro Pizzero**, **Leyenda**, **Clan Leader**
- [ ] Canal `#nostalgia-2016`

### Día 12: Integración de Bot y Autenticación Discord
- [ ] Bot en Node.js para vincular cuentas (`/vincular [código]`)
- [ ] Bloqueo previo al juego para asegurar cuentas reales
- [ ] Expulsión automática de baneados

### Días 13 y 14: Contacto Nostálgico (Veteranos)
- [ ] Mensaje respetuoso al creador/admins originales de 2016 contándoles sobre el revival
- [ ] Búsqueda activa de ex-jugadores y líderes de clanes viejos en grupos y redes

### Día 15: Difusión Visual (TikTok / Shorts)
- [ ] Teasers de 15 a 30 segundos del Royal Rumble en la grúa
- [ ] Teasers de escapadas del Némesis

---

## FASE 4: Beta, Servidor y Gran Apertura (Días 16 al 20)

### Día 16: Beta Cerrada (Stress Test)
- [ ] Pruebas con 10-15 amigos/beta testers
- [ ] Verificar daño entre humanos/zombies
- [ ] Verificar la mordida
- [ ] Verificar rendimiento de Manos de Fuego

### Día 17: Ajustes de Balance y Corrección de Bugs
- [ ] Corregir detalles de código según el feedback de la beta

### Día 18: Montaje en VPS (OVHcloud Game)
- [ ] Instalación en VPS (Virginia/Brasil) con protección Anti-DDoS Game activa
- [ ] Prueba de latencia desde Argentina y resto de LATAM

### Día 19: Anuncio de Fecha de Apertura (Hype Final)
- [ ] Notificación masiva en Discord
- [ ] Lanzamiento de evento de bienvenida

### Día 20: GRAN APERTURA 🚀
- [ ] Apertura oficial del servidor con **Panoptipizza** liderando el primer Royal Rumble

---

## 🔐 Esquema de Seguridad y Sostenibilidad

### Protección Anti-Cheat en 3 Capas
| Capa | Descripción |
|---|---|
| **Server-Side** | Verificación de vectores de disparo (`OnPlayerWeaponShot`) y límites de posición con open.mp |
| **Hardware & Identidad** | Ban por GPCI + ID única de Discord + rango de subred IP |
| **Verificación OAuth2** | Cuenta activa de Discord obligatoria para evitar reingresos infinitos |

### Autosustentabilidad Financiera ($30 - $45 USD/mes)
- Cobertura del host con **10 VIPs mensuales** ($3 - $5 USD) o alquiler de **pases de clanes** ($20 - $80 USD)
- Pagos automatizados vía webhooks en PHP/Laravel con **Mercado Pago** y **PayPal**
