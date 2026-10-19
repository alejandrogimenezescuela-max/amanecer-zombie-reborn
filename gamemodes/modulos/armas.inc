// ------------------------------------------------------------
//  /armas: tienda de armas, copiada del servidor original.
//  Humanos: "Tienda de humanos" con armas de fuego y un submenu de
//  armas blancas. Zombies: solo la motosierra.
//  Cada compra de un arma de fuego da MUNICION_COMPRA balas que se suman
//  a las que ya tenia, asi que tambien sirve para comprar municion.
//  No se venden: escopeta de combate, recortada, RPG ni Uzi.
// ------------------------------------------------------------

#define MUNICION_COMPRA         500
#define PRECIO_MOTOSIERRA       3000

// Tienda de humanos, en el orden del servidor original.
// La ultima fila del menu ("Armas blancas") abre el submenu.
static const WEAPON:gArmasHumano[] =
{
    WEAPON_DEAGLE, WEAPON_SHOTGUN, WEAPON_MP5, WEAPON_M4, WEAPON_AK47, WEAPON_COLT45,
    WEAPON_TEC9, WEAPON_SNIPER, WEAPON_SILENCED, WEAPON_RIFLE
};
// AK-47: $8.000 (en la captura del original el precio estaba tapado; va entre el MP5 y la M4)
static const gPreciosHumano[] = { 5000, 7000, 5000, 10000, 8000, 1000, 4000, 15000, 1000, 5000 };
static const gNombresHumano[][] =
{
    "Desert Eagle", "Escopeta", "MP5-K MG", "M4", "AK-47", "9mm",
    "Tec-9", "Sniper", "Silenciada", "Rifle"
};

// Armas blancas (precios propios: no estaban en la captura del original)
static const WEAPON:gArmasBlancas[] =
{
    WEAPON_BRASSKNUCKLE, WEAPON_GOLFCLUB, WEAPON_NITESTICK, WEAPON_KNIFE,
    WEAPON_BAT, WEAPON_SHOVEL, WEAPON_POOLSTICK, WEAPON_KATANA
};
static const gPreciosBlancas[] = { 300, 500, 800, 1000, 500, 500, 500, 2000 };
static const gNombresBlancas[][] =
{
    "Nudillos", "Palo de golf", "Porra", "Cuchillo",
    "Bate", "Pala", "Palo de billar", "Katana"
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

// "Nombre - $5.000" por renglon, como en el original
static ArmarLista(const nombres[][], const precios[], cantidad, cuerpo[], largo)
{
    new precio[20], linea[48];
    cuerpo[0] = EOS;
    for (new i = 0; i < cantidad; i++)
    {
        FormatearDinero(precios[i], precio);
        format(linea, sizeof(linea), "%s%s - %s", (i == 0) ? ("") : ("\n"), nombres[i], precio);
        strcat(cuerpo, linea, largo);
    }
}

static MostrarTienda(playerid)
{
    new cuerpo[512];
    if (gJugador[playerid][jBando] == BANDO_ZOMBIE)
    {
        new precio[20];
        FormatearDinero(PRECIO_MOTOSIERRA, precio);
        format(cuerpo, sizeof(cuerpo), "Motosierra - %s", precio);
        ShowPlayerDialog(playerid, DIALOGO_ARMAS, DIALOG_STYLE_LIST, "Tienda de zombies", cuerpo, "Aceptar", "Cancelar");
        return;
    }

    ArmarLista(gNombresHumano, gPreciosHumano, sizeof(gNombresHumano), cuerpo, sizeof(cuerpo));
    strcat(cuerpo, "\nArmas blancas");
    ShowPlayerDialog(playerid, DIALOGO_ARMAS, DIALOG_STYLE_LIST, "Tienda de humanos", cuerpo, "Aceptar", "Cancelar");
}

static MostrarArmasBlancas(playerid)
{
    new cuerpo[384];
    ArmarLista(gNombresBlancas, gPreciosBlancas, sizeof(gNombresBlancas), cuerpo, sizeof(cuerpo));
    ShowPlayerDialog(playerid, DIALOGO_ARMAS_BLANCAS, DIALOG_STYLE_LIST, "Armas blancas", cuerpo, "Aceptar", "Volver");
}

// Devuelve 1 si el dialogo era de este modulo
Armas_AlResponderDialogo(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOGO_ARMAS && dialogid != DIALOGO_ARMAS_BLANCAS) return 0;
    if (!gJugador[playerid][jEnJuego]) return 1;

    // Submenu de armas blancas: "Volver" regresa a la tienda
    if (dialogid == DIALOGO_ARMAS_BLANCAS)
    {
        if (!response)
        {
            MostrarTienda(playerid);
            return 1;
        }
        if (gJugador[playerid][jBando] == BANDO_ZOMBIE || listitem < 0 || listitem >= sizeof(gArmasBlancas)) return 1;

        new item[40];
        format(item, sizeof(item), "%s", gNombresBlancas[listitem]);
        if (Dinero_Cobrar(playerid, gPreciosBlancas[listitem], item)) GivePlayerWeapon(playerid, gArmasBlancas[listitem], 1);
        return 1;
    }

    if (!response) return 1;

    // El bando pudo cambiar con el menu abierto (lo infectaron): se valida de nuevo
    if (gJugador[playerid][jBando] == BANDO_ZOMBIE)
    {
        if (listitem == 0 && Dinero_Cobrar(playerid, PRECIO_MOTOSIERRA, "una motosierra"))
        {
            GivePlayerWeapon(playerid, WEAPON_CHAINSAW, 1);
        }
        return 1;
    }

    if (listitem == sizeof(gArmasHumano))
    {
        MostrarArmasBlancas(playerid);
        return 1;
    }
    if (listitem < 0 || listitem > sizeof(gArmasHumano)) return 1;

    new item[48];
    format(item, sizeof(item), "%s (+%d balas)", gNombresHumano[listitem], MUNICION_COMPRA);
    if (Dinero_Cobrar(playerid, gPreciosHumano[listitem], item))
    {
        // Si ya la tenia, las balas se suman a las que le quedaban
        GivePlayerWeapon(playerid, gArmasHumano[listitem], MUNICION_COMPRA);
    }
    return 1;
}
