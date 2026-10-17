// ------------------------------------------------------------
//  Interfaz: logo del servidor arriba del minimapa
//  SA-MP 0.3.7 no puede mostrar imagenes propias: el logo se
//  recrea con texto ("Amanecer" blanco, "Zombie" rojo, "REBORN" gris).
// ------------------------------------------------------------

// Centro del logo (pantalla de 640 x 448; el minimapa ocupa ~x 37-137, y 340-423)
const Float:LOGO_X = 87.0;
const Float:LOGO_Y = 290.0;

#define LOGO_COLOR_REBORN       0xD8D8D0FF      // gris claro
#define LOGO_COLOR_BORDE        0x000000FF

static Text:gLogoTitulo;
static Text:gLogoReborn;

Interfaz_Crear()
{
    // Fuente 0: letra caligrafica, lo mas parecido a la gotica del logo.
    // ~w~ y ~r~ cambian el color dentro del mismo texto.
    gLogoTitulo = TextDrawCreate(LOGO_X, LOGO_Y, "~w~Amanecer ~r~Zombie");
    TextDrawFont(gLogoTitulo, TEXT_DRAW_FONT_BECKETT_REGULAR);
    TextDrawLetterSize(gLogoTitulo, 0.40, 1.8);
    TextDrawAlignment(gLogoTitulo, TEXT_DRAW_ALIGN_CENTER);
    TextDrawSetProportional(gLogoTitulo, true);
    TextDrawSetOutline(gLogoTitulo, 1);
    TextDrawBackgroundColour(gLogoTitulo, LOGO_COLOR_BORDE);

    gLogoReborn = TextDrawCreate(LOGO_X, LOGO_Y + 18.0, "REBORN");
    TextDrawFont(gLogoReborn, TEXT_DRAW_FONT_2);
    TextDrawLetterSize(gLogoReborn, 0.36, 1.4);
    TextDrawAlignment(gLogoReborn, TEXT_DRAW_ALIGN_CENTER);
    TextDrawSetProportional(gLogoReborn, true);
    TextDrawColour(gLogoReborn, LOGO_COLOR_REBORN);
    TextDrawSetOutline(gLogoReborn, 1);
    TextDrawBackgroundColour(gLogoReborn, LOGO_COLOR_BORDE);
}

// Se muestra al spawnear (en el selector y el login no hay minimapa), salvo que
// el jugador lo haya apagado en /config
Interfaz_Mostrar(playerid)
{
    if (Ajustes_Activado(playerid, CFG_SIN_LOGO))
    {
        TextDrawShowForPlayer(playerid, gLogoTitulo);
        TextDrawShowForPlayer(playerid, gLogoReborn);
    }
    else
    {
        TextDrawHideForPlayer(playerid, gLogoTitulo);
        TextDrawHideForPlayer(playerid, gLogoReborn);
    }
}
