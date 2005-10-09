--TEST--
PostgreSQL create db
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// create test table 

include('config.inc');

$db = pg_connect($conn_str);
$res = @pg_query($db, "SELECT * FROM ".$table_name);
if (!@pg_num_rows($res)) 
{
	@pg_query($db,$table_def); // Create table here
	for ($i=0; $i < $num_test_record; $i++) {
		pg_query($db,"INSERT INTO ".$table_name." VALUES ($i, 'ABC');");
	}
}
else {
	echo pg_last_error()."\n";
}

pg_close($db);

echo "OK";
?>
--EXPECT--
OK
