// ------------------------------------------------------------
//  /armas: tienda de armas. Cada compra da MUNICION_COMPRA balas que se
//  suman a las que ya tenia, asi que tambien sirve para comprar municion.
//  Los humanos tienen armas de fuego; los zombies solo la motosierra.
//  No se venden: escopeta de combate, recortada, RPG ni Uzi.
// ------------------------------------------------------------

#define MUNICION_COMPRA         500
#define PRECIO_MOTOSIERRA       10000

// Humanos: en el orden en que aparecen en el menu
static const WEAPON:gArmasHumano[] =
{
    WEAPON_SHOTGUN, WEAPON_M4, WEAPON_RIFLE, WEAPON_TEC9,
    WEAPON_SNIPER, WEAPON_DEAGLE, WEAPON_AK47, WEAPON_MP5
};
static const gPreciosHumano[] = { 15000, 35000, 25000, 50000, 80000, 15000, 22000, 8000 };
static const gNombresHumano[][] =
{
    "Escopeta", "M4", "Rifle de caza", "TEC-9",
    "Francotirador", "Desert Eagle", "AK-47", "MP5"
};

// Devuelve 1 si el comando era de este modulo
Armas_AlComando(playerid, const cmdtext[])
{
    new comando[16];
    ObtenerParametro(cmdtext, 0, comando);
    if (strcmp(comando, "/armas", true)) return 0;

    if (gJugador[playerid][jEnJuego]) MostrarTienda(playerid);
    return 1;
}

static MostrarTienda(playerid)
{
    new dinero[20], titulo[48], cuerpo[512], precio[20], linea[64];
    FormatearDinero(Dinero_Obtener(playerid), dinero);
    format(titulo, sizeof(titulo), "Armas  -  Tu dinero: %s", dinero);

    if (gJugador[playerid][jBando] == BANDO_ZOMBIE)
    {
        FormatearDinero(PRECIO_MOTOSIERRA, precio);
        format(cuerpo, sizeof(cuerpo), "Arma\tPrecio\n{FFFFFF}Motosierra\t%s", precio);
    }
    else
    {
        cuerpo = "Arma\tPrecio\tMunicion";
        for (new i = 0; i < sizeof(gArmasHumano); i++)
        {
            FormatearDinero(gPreciosHumano[i], precio);
            format(linea, sizeof(linea), "\n{FFFFFF}%s\t%s\t{AAAAAA}+%d balas", gNombresHumano[i], precio, MUNICION_COMPRA);
            strcat(cuerpo, linea);
        }
    }
    ShowPlayerDialog(playerid, DIALOGO_ARMAS, DIALOG_STYLE_TABLIST_HEADERS, titulo, cuerpo, "Comprar", "Cerrar");
}

// Devuelve 1 si el dialogo era de este modulo
Armas_AlResponderDialogo(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOGO_ARMAS) return 0;
    if (!response || !gJugador[playerid][jEnJuego]) return 1;

    // El bando pudo cambiar con el menu abierto (lo infectaron): se valida de nuevo
    if (gJugador[playerid][jBando] == BANDO_ZOMBIE)
    {
        if (listitem == 0 && Dinero_Cobrar(playerid, PRECIO_MOTOSIERRA, "una motosierra"))
        {
            GivePlayerWeapon(playerid, WEAPON_CHAINSAW, 1);
        }
        return 1;
    }

    if (listitem < 0 || listitem >= sizeof(gArmasHumano)) return 1;

    new item[48];
    format(item, sizeof(item), "%s (+%d balas)", gNombresHumano[listitem], MUNICION_COMPRA);
    if (Dinero_Cobrar(playerid, gPreciosHumano[listitem], item))
    {
        // Si ya la tenia, las balas se suman a las que le quedaban
        GivePlayerWeapon(playerid, gArmasHumano[listitem], MUNICION_COMPRA);
    }
    return 1;
}
