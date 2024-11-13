--TEST--
PostgreSQL create db
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
// create test table

include('inc/config.inc');
$table_name = 'table_01createdb';
$table_name_92 = 'table_01createdb_92';
$view_name   = "view_01createdb";

$db = pg_connect($conn_str);
if (!($q = @pg_query($db, "SELECT * FROM ".$table_name)) || !@pg_num_rows($q))
{
    pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)"); // Create table here
}
else {
    echo pg_last_error()."\n";
}

$v = pg_version($db);
if (version_compare($v['server'], '9.2', '>=') && (!($q = @pg_query($db, "SELECT * FROM ".$table_name_92)) || !@pg_num_rows($q)))
{
    pg_query($db, "CREATE TABLE {$table_name_92} (textary text[],  jsn json)"); // Create table here
}
else {
    echo pg_last_error()."\n";
}

// Create view here
pg_query($db, "CREATE VIEW {$view_name} AS SELECT * FROM {$table_name}");

pg_close($db);

echo "OK";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = 'table_01createdb';
$table_name_92 = 'table_01createdb_92';

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name} cascade");
pg_query($db, "DROP TABLE IF EXISTS {$table_name_92} cascade");
?>
--EXPECT--
OK
