// ------------------------------------------------------------
//  Bot de Discord de Amanecer Zombie Reborn.
//  /vincular <codigo>: el jugador pega el codigo que le muestra el juego
//  despues del login. El bot guarda su ID de Discord en users.discord_id,
//  le da el rol de vinculado y el servidor del juego lo deja entrar.
//  Reglas: un Discord = una cuenta, y un Discord baneado no vincula.
// ------------------------------------------------------------

import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { Client, Events, GatewayIntentBits, MessageFlags, REST, Routes, SlashCommandBuilder } from 'discord.js';
import mysql from 'mysql2/promise';

const carpeta = dirname(fileURLToPath(import.meta.url));
process.loadEnvFile(join(carpeta, '.env'));

const { DISCORD_TOKEN, DISCORD_CLIENT_ID, DISCORD_GUILD_ID, DISCORD_ROL_VINCULADO } = process.env;
for (const [nombre, valor] of Object.entries({ DISCORD_TOKEN, DISCORD_CLIENT_ID, DISCORD_GUILD_ID })) {
    if (!valor) {
        console.error(`[Bot] Falta ${nombre} en bot/.env`);
        process.exit(1);
    }
}

// Mismas credenciales que el servidor (mysql.ini: "clave = valor")
function leerMysqlIni() {
    const datos = {};
    for (const linea of readFileSync(join(carpeta, '..', 'mysql.ini'), 'utf8').split(/\r?\n/)) {
        const igual = linea.indexOf('=');
        if (igual > 0) datos[linea.slice(0, igual).trim()] = linea.slice(igual + 1).trim();
    }
    return datos;
}

const ini = leerMysqlIni();
const db = mysql.createPool({
    host: ini.hostname,
    port: ini.port ? Number(ini.port) : 3306,
    user: ini.username,
    password: ini.password,
    database: ini.database,
    connectionLimit: 4,
});

// ------------------------------------------------------------
//  Comandos
// ------------------------------------------------------------

const comandos = [
    new SlashCommandBuilder()
        .setName('vincular')
        .setDescription('Vincula tu cuenta de Amanecer Zombie con tu Discord')
        .addStringOption((opcion) => opcion
            .setName('codigo')
            .setDescription('El codigo que te muestra el juego al iniciar sesion')
            .setRequired(true)
            .setMinLength(6)
            .setMaxLength(6)),
];

// Se registran solo en tu servidor: aparecen al instante (los globales tardan)
async function registrarComandos() {
    const rest = new REST().setToken(DISCORD_TOKEN);
    await rest.put(Routes.applicationGuildCommands(DISCORD_CLIENT_ID, DISCORD_GUILD_ID), {
        body: comandos.map((c) => c.toJSON()),
    });
}

async function vincular(interaccion) {
    const codigo = interaccion.options.getString('codigo', true).trim().toUpperCase();
    const discordId = interaccion.user.id;

    const [baneos] = await db.query(
        `SELECT 1 FROM banlist WHERE activo = 1 AND (expira IS NULL OR expira > NOW()) AND discord_id = ? LIMIT 1`,
        [discordId]);
    if (baneos.length) return 'Tu Discord esta baneado del servidor.';

    const [propias] = await db.query('SELECT nombre FROM users WHERE discord_id = ? LIMIT 1', [discordId]);
    if (propias.length) return `Tu Discord ya esta vinculado a la cuenta **${propias[0].nombre}**. Solo se permite una cuenta por Discord.`;

    const [codigos] = await db.query(
        `SELECT c.user_id, u.nombre, u.discord_id FROM discord_codigos c JOIN users u ON u.id = c.user_id
         WHERE c.codigo = ? AND c.expira > NOW() LIMIT 1`,
        [codigo]);
    if (!codigos.length) return 'Ese codigo no existe o ya vencio. Volve a entrar al juego para que te muestre uno nuevo.';

    const { user_id: userId, nombre } = codigos[0];
    if (codigos[0].discord_id) return `La cuenta **${nombre}** ya esta vinculada a otro Discord.`;

    // La condicion "discord_id IS NULL" evita pisar una vinculacion hecha al mismo tiempo
    const [resultado] = await db.query('UPDATE users SET discord_id = ? WHERE id = ? AND discord_id IS NULL', [discordId, userId]);
    if (resultado.affectedRows === 0) return `La cuenta **${nombre}** ya esta vinculada a otro Discord.`;
    await db.query('DELETE FROM discord_codigos WHERE user_id = ?', [userId]);

    let avisoRol = '';
    if (DISCORD_ROL_VINCULADO && interaccion.inGuild()) {
        try {
            const miembro = await interaccion.guild.members.fetch(discordId);
            await miembro.roles.add(DISCORD_ROL_VINCULADO, `Vinculo la cuenta ${nombre}`);
        } catch (error) {
            console.error(`[Bot] No se pudo dar el rol a ${interaccion.user.tag}:`, error.message);
            avisoRol = '\n(No se pudo darte el rol: avisale a un admin.)';
        }
    }

    console.log(`[Bot] ${interaccion.user.tag} (${discordId}) vinculo la cuenta ${nombre} (id ${userId})`);
    return `Listo, tu Discord quedo vinculado a la cuenta **${nombre}**. Ya podes jugar.${avisoRol}`;
}

// ------------------------------------------------------------
//  Cliente
// ------------------------------------------------------------

const cliente = new Client({ intents: [GatewayIntentBits.Guilds] });

cliente.once(Events.ClientReady, async (listo) => {
    await registrarComandos();
    console.log(`[Bot] Conectado como ${listo.user.tag}. Comando /vincular registrado.`);
});

cliente.on(Events.InteractionCreate, async (interaccion) => {
    if (!interaccion.isChatInputCommand() || interaccion.commandName !== 'vincular') return;

    // La respuesta solo la ve el que uso el comando (asi nadie ve el codigo ajeno)
    await interaccion.deferReply({ flags: MessageFlags.Ephemeral });
    try {
        await interaccion.editReply(await vincular(interaccion));
    } catch (error) {
        console.error('[Bot] Error en /vincular:', error);
        await interaccion.editReply('Hubo un error al vincular. Proba de nuevo en un rato.');
    }
});

cliente.login(DISCORD_TOKEN);
