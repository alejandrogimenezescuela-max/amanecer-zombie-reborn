-- ============================================================
--  Amanecer Zombie Reborn - Esquema de base de datos (MySQL / MariaDB)
--  Dia 1 del ROADMAP: tablas users, banlist y clans
-- ============================================================

CREATE DATABASE IF NOT EXISTS amanecer_zombie
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_general_ci;

USE amanecer_zombie;

-- ------------------------------------------------------------
--  Clanes (se crea primero porque users la referencia)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS clans (
    id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
    nombre          VARCHAR(32)  NOT NULL,
    tag             VARCHAR(6)   NOT NULL,
    color           INT UNSIGNED NOT NULL DEFAULT 0xFFFFFFFF,
    lider_id        INT UNSIGNED NULL,
    oficial         TINYINT(1)   NOT NULL DEFAULT 0,      -- 1 = uno de los slots oficiales con base
    base_x          FLOAT        NULL,
    base_y          FLOAT        NULL,
    base_z          FLOAT        NULL,
    fecha_creacion  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    UNIQUE KEY uq_clans_nombre (nombre),
    UNIQUE KEY uq_clans_tag (tag)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
--  Usuarios
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS users (
    id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
    nombre          VARCHAR(24)  NOT NULL,
    password_hash   VARCHAR(255) NOT NULL,
    salt            VARCHAR(32)  NOT NULL DEFAULT '',

    -- Juego
    bando           ENUM('humano', 'zombie') NOT NULL DEFAULT 'humano',
    skin            SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    vida            FLOAT        NOT NULL DEFAULT 100.0,
    pos_x           FLOAT        NULL,
    pos_y           FLOAT        NULL,
    pos_z           FLOAT        NULL,
    angulo          FLOAT        NULL,
    score           INT          NOT NULL DEFAULT 0,
    dinero          INT          NOT NULL DEFAULT 0,      -- el gamemode la agrega si falta
    cfg_clima       TINYINT      NOT NULL DEFAULT -1,     -- /config (-1 = el del servidor)
    cfg_hora        TINYINT      NOT NULL DEFAULT -1,
    cfg_flags       INT          NOT NULL DEFAULT 0,      -- opciones desactivadas (CFG_SIN_*)
    kills           INT UNSIGNED NOT NULL DEFAULT 0,
    muertes         INT UNSIGNED NOT NULL DEFAULT 0,

    -- Rangos
    admin_nivel     TINYINT UNSIGNED NOT NULL DEFAULT 0,
    vip_nivel       TINYINT UNSIGNED NOT NULL DEFAULT 0,
    vip_expira      DATETIME     NULL,

    -- Clan
    clan_id         INT UNSIGNED NULL,
    clan_rango      TINYINT UNSIGNED NOT NULL DEFAULT 0,

    -- Identificacion (Dia 10 y Dia 12)
    ultimo_gpci     VARCHAR(41)  NULL,
    ultima_ip       VARCHAR(16)  NULL,
    discord_id      VARCHAR(20)  NULL,

    fecha_registro  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    ultimo_ingreso  DATETIME     NULL,

    PRIMARY KEY (id),
    UNIQUE KEY uq_users_nombre (nombre),
    UNIQUE KEY uq_users_discord (discord_id),
    KEY idx_users_gpci (ultimo_gpci),
    KEY idx_users_ip (ultima_ip),
    CONSTRAINT fk_users_clan FOREIGN KEY (clan_id) REFERENCES clans (id)
        ON DELETE SET NULL
) ENGINE=InnoDB;

-- El lider del clan es un usuario
ALTER TABLE clans
    ADD CONSTRAINT fk_clans_lider FOREIGN KEY (lider_id) REFERENCES users (id)
        ON DELETE SET NULL;

-- ------------------------------------------------------------
--  Spawns por bando (el gamemode tambien la crea si no existe)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spawns (
    id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
    bando           ENUM('humano', 'zombie') NOT NULL,
    pos_x           FLOAT        NOT NULL,
    pos_y           FLOAT        NOT NULL,
    pos_z           FLOAT        NOT NULL,
    angulo          FLOAT        NOT NULL DEFAULT 0,
    PRIMARY KEY (id)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
--  Lista de baneos (por cuenta, IP, subred, GPCI y Discord)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS banlist (
    id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
    user_id         INT UNSIGNED NULL,
    nombre          VARCHAR(24)  NULL,
    ip              VARCHAR(16)  NULL,
    subred          VARCHAR(16)  NULL,      -- ej: 190.12.34
    gpci            VARCHAR(41)  NULL,
    discord_id      VARCHAR(20)  NULL,
    motivo          VARCHAR(128) NOT NULL,
    admin_nombre    VARCHAR(24)  NOT NULL,
    fecha           DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    expira          DATETIME     NULL,      -- NULL = permanente
    activo          TINYINT(1)   NOT NULL DEFAULT 1,
    PRIMARY KEY (id),
    KEY idx_ban_ip (ip),
    KEY idx_ban_subred (subred),
    KEY idx_ban_gpci (gpci),
    KEY idx_ban_discord (discord_id),
    CONSTRAINT fk_ban_user FOREIGN KEY (user_id) REFERENCES users (id)
        ON DELETE SET NULL
) ENGINE=InnoDB;
