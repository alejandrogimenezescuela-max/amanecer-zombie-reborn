#include <open.mp>

main()
{
    print("Amanecer Zombie Reborn - gamemode cargado");
}

public OnGameModeInit()
{
    SetGameModeText("Zombie Survival");

    // Clase de jugador: skin 0, aparece en Grove Street (Los Santos)
    AddPlayerClass(0, 2495.0, -1687.0, 13.5, 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    return 1;
}

public OnPlayerConnect(playerid)
{
    SendClientMessage(playerid, 0x00FF00FF, "Bienvenido a Amanecer Zombie Reborn.");
    SendClientMessage(playerid, 0xFFFFFFFF, "El mundo cay?. Sobreviv? como puedas...");
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    // Lo que se ve en la pantalla de selecci?n de personaje
    SetPlayerPos(playerid, 2495.0, -1687.0, 13.5);
    SetPlayerFacingAngle(playerid, 0.0);
    SetPlayerCameraPos(playerid, 2495.0, -1680.0, 15.0);
    SetPlayerCameraLookAt(playerid, 2495.0, -1687.0, 13.5);
    return 1;
}

public OnPlayerSpawn(playerid)
{
    SendClientMessage(playerid, 0xFF0000FF, "Cuidado: los zombies est?n cerca.");
    return 1;
}