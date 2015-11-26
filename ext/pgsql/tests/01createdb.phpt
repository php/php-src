--TEST--
PostgreSQL create db
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// create test table

include('config.inc');

$db = pg_connect($conn_str);
if (!@pg_num_rows(@pg_query($db, "SELECT * FROM ".$table_name)))
{
	pg_query($db,$table_def); // Create table here
	for ($i=0; $i < $num_test_record; $i++) {
		pg_query($db,"INSERT INTO ".$table_name." VALUES ($i, 'ABC');");
	}
}
else {
	echo pg_last_error()."\n";
}

$v = pg_version();
if (version_compare($v['server'], '9.2', '>=') && !@pg_num_rows(@pg_query($db, "SELECT * FROM ".$table_name_92)))
{
	pg_query($db,$table_def_92); // Create table here
}
else {
	echo pg_last_error()."\n";
}

pg_close($db);

echo "OK";
?>
--EXPECT--
OK
