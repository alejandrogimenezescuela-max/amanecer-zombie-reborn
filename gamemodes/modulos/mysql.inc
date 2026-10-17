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

forward AlRevisarColumna(tabla[], columna[], definicion[]);

// Agrega la columna si la base es anterior al sistema que la usa (MySQL 8 no
// tiene ADD COLUMN IF NOT EXISTS, asi que primero se pregunta si existe)
MySQL_AsegurarColumna(const tabla[], const columna[], const definicion[])
{
    new consulta[256];
    mysql_format(gMySQL, consulta, sizeof(consulta),
        "SELECT COUNT(*) FROM information_schema.COLUMNS \
         WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = '%e' AND COLUMN_NAME = '%e'",
        tabla, columna);
    mysql_tquery(gMySQL, consulta, "AlRevisarColumna", "sss", tabla, columna, definicion);
}

public AlRevisarColumna(tabla[], columna[], definicion[])
{
    new existe;
    cache_get_value_index_int(0, 0, existe);
    if (!existe)
    {
        new consulta[256];
        format(consulta, sizeof(consulta), "ALTER TABLE %s ADD COLUMN %s %s", tabla, columna, definicion);
        mysql_tquery(gMySQL, consulta);
        printf("[MySQL] Columna %s.%s creada.", tabla, columna);
    }
    return 1;
}

MySQL_Cerrar()
{
    // mysql_close espera a que terminen las consultas pendientes
    mysql_close(gMySQL);
}
