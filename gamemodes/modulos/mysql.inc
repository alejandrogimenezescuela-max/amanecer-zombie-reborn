// ------------------------------------------------------------
//  Conexion a MySQL (credenciales en mysql.ini)
// ------------------------------------------------------------

new MySQL:gMySQL;

MySQL_Conectar()
{
    // Solo errores y advertencias en logs/plugins/mysql.log. Con ALL se escribe
    // cada consulta completa (hashes, salts e IPs de los jugadores): usar solo para depurar.
    mysql_log(ERROR | WARNING);

    gMySQL = mysql_connect_file();
    if (gMySQL == MYSQL_INVALID_HANDLE || mysql_errno(gMySQL) != 0)
    {
        printf("[MySQL] ERROR %d: no se pudo conectar. Revisa mysql.ini", mysql_errno(gMySQL));
    }
    else
    {
        print("[MySQL] Conectado a amanecer_zombie.");
    }
}

MySQL_Cerrar()
{
    // mysql_close espera a que terminen las consultas pendientes
    mysql_close(gMySQL);
}
