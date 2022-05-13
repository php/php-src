--TEST--
PostgreSQL create db
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// create test table

include('config.inc');

$db = pg_connect($conn_str);
if (!($q = @pg_query($db, "SELECT * FROM ".$table_name)) || !@pg_num_rows($q))
{
    pg_query($db,$table_def); // Create table here
    for ($i=0; $i < $num_test_record; $i++) {
        pg_query($db,"INSERT INTO ".$table_name." VALUES ($i, 'ABC');");
    }
}
else {
    echo pg_last_error()."\n";
}

$v = pg_version($db);
if (version_compare($v['server'], '9.2', '>=') && (!($q = @pg_query($db, "SELECT * FROM ".$table_name_92)) || !@pg_num_rows($q)))
{
    pg_query($db,$table_def_92); // Create table here
}
else {
    echo pg_last_error()."\n";
}

// Create view here
pg_query($db,$view_def);

pg_close($db);

echo "OK";
?>
--EXPECT--
OK
