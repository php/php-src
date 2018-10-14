--TEST--
Bug #27597 (pg_fetch_array not returning false)
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/config.inc');

$dbh = @pg_connect($conn_str);
if (!$dbh) {
	die ("Could not connect to the server");
}

@pg_query("DROP TABLE id");
pg_query("CREATE TABLE id (id INT)");

for ($i=0; $i<4; $i++) {
	pg_query("INSERT INTO id (id) VALUES ($i)");
}

function xi_fetch_array($res, $type = PGSQL_ASSOC) {
	$a = pg_fetch_array($res, NULL, $type) ;
	return $a ;
}

$res = pg_query("SELECT * FROM id");
$i = 0; // endless-loop protection
while($row = xi_fetch_array($res)) {
	print_r($row);
	if ($i++ > 4) {
		echo "ENDLESS-LOOP";
		exit(1);
	}
}

pg_close($dbh);

?>
===DONE===
--EXPECTF--
Array
(
    [id] => 0
)
Array
(
    [id] => 1
)
Array
(
    [id] => 2
)
Array
(
    [id] => 3
)
===DONE===
