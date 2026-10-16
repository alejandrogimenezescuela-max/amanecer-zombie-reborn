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
- [x] Clima de niebla, skin 155 para admins RCON, `/cure` y `/virus`
- [ ] Cura en el skatepark de Los Santos (por ahora solo la cámara del login lo muestra)

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
