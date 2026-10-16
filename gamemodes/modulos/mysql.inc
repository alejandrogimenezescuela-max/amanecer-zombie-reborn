// ------------------------------------------------------------
//  Conexion a MySQL (credenciales en mysql.ini)
// ------------------------------------------------------------

new MySQL:gMySQL;

MySQL_Conectar()
{
    // Registra todo lo que pasa con MySQL en logs/plugins/mysql.log
    mysql_log(ALL);

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
