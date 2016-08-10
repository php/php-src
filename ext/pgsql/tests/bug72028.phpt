--TEST--
Bug #72028 pg_query_params(): NULL converts to empty string
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// create test table

include('config.inc');

$conn = pg_connect($conn_str);

$table = "bug72028_" . md5(uniqid(time()));

pg_query("CREATE TABLE $table (value TEXT, details TEXT);");

$sql = "INSERT INTO $table (value, details) VALUES ($1, $2)";

$params = array(null, "insert before looping with a reference");
$result = pg_query_params($conn, $sql, $params);

$params2 = array(null, "insert after looping with a reference");
foreach ($params2 as &$p) {
    // doing nothing
}
unset($p);

$result = pg_query_params($conn, $sql, $params2);

$r = pg_query("SELECT * FROM $table");
while (false !== ($i = pg_fetch_assoc($r))) {
	var_dump($i);
}

pg_query("DROP TABLE $table");

?>
==DONE==
--EXPECT--
array(2) {
  ["value"]=>
  NULL
  ["details"]=>
  string(38) "insert before looping with a reference"
}
array(2) {
  ["value"]=>
  NULL
  ["details"]=>
  string(37) "insert after looping with a reference"
}
==DONE==
