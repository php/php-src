<?php

include 'config.inc';
define('FILE_NAME', './php.gif');

// pg_escape_string() test
$before = "ABC\\ABC\'";
$expect  = "ABC\\\\ABC\\'";
$after = pg_escape_string($before);
if ($expect === $after) {
	echo "pg_escape_string() is Ok\n";
}
else {
	echo "pg_escape_string() is NOT Ok\n";
	var_dump($before);
	var_dump($after);
	var_dump($expect);
}

// pg_escape_bytea() test
$before = "ABC\\ABC";
$expect  = "ABC\\\\\\\\ABC";
$after  = pg_escape_bytea($before);
if ($expect === $after) {
	echo "pg_escape_bytea() is Ok\n";
}
else {
	echo "pg_escape_byte() is NOT Ok\n";
	var_dump($before);
	var_dump($after);
	var_dump($expect);
}

// Test using database
$fp = fopen(FILE_NAME,'r');
$data = fread($fp, filesize(FILE_NAME));

$db = pg_connect($conn_str);
$escaped_data = pg_escape_bytea($data);
//$out = fopen('php.gif_escaped','w');
//fwrite($out, $escaped_data);
//exit;
pg_query("DELETE FROM ".$table_name." WHERE num = -2;");
$sql = "INSERT INTO ".$table_name." (num, bin) VALUES (-2, '".$escaped_data."');";
pg_query($db, $sql);
$sql = "SELECT * FROM ".$table_name." WHERE num = -2";
$result = pg_query($db, $sql);
$row = pg_fetch_row($result, 0);
if ($data === $row['bin']) {
	echo "pg_escape_bytea actually works\n";
}
else {
	echo "pg_escape_bytea is broken\n";
}

?>
