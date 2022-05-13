--TEST--
PostgreSQL connection
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// connection function tests

include('config.inc');

$db = pg_pconnect($conn_str);
var_dump($db);

if (pg_connection_status($db) != PGSQL_CONNECTION_OK)
{
    echo "pg_connection_status() error\n";
}
if (!pg_connection_reset($db))
{
    echo "pg_connection_reset() error\n";
}
if (pg_connection_busy($db))
{
    echo "pg_connection_busy() error\n";
}
if (function_exists('pg_transaction_status')) {
    if (pg_transaction_status($db) != PGSQL_TRANSACTION_IDLE)
    {
        echo "pg_transaction_status() error\n";
    }
}
if (false === pg_host($db))
{
    echo "pg_host() error\n";
}
if (!pg_dbname($db))
{
    echo "pg_dbname() error\n";
}
if (!pg_port($db))
{
    echo "pg_port() error\n";
}
if (pg_tty($db))
{
    echo "pg_tty() error\n";
}
if (pg_options($db))
{
    echo "pg_options() error\n";
}

pg_close($db);

?>
--EXPECTF--
object(PgSql\Connection)#%d (0) {
}
