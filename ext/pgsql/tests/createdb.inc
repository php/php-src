<?php
// create test table 

include('config.inc');

$db = pg_connect($conn_str);
if (!@pg_num_rows(@pg_query($db, "SELECT * FROM ".$table_name))) 
{
	@pg_query($db,$table_def);
	for ($i=0; $i < $num_test_record; $i++) {
		pg_query($db,"INSERT INTO php_pgsql_test VALUES ($i, 'ABC');");
	}
}
else {
	echo pg_last_error()."\n";
}

pg_close($db);

echo "OK";

?>