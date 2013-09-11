--TEST--
PostgreSQL escape functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include 'config.inc';
define('FILE_NAME', dirname(__FILE__) . '/php.gif');

// pg_escape_string() test
$before = "ABC\\ABC\'";
$expect  = "ABC\\\\ABC\\'";
$expect2  = "ABC\\\\ABC\\\\''"; //the way escape string differs from PostgreSQL 9.0
$after = pg_escape_string($before);
if ($expect === $after || $expect2 === $after) {
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
$data = file_get_contents(FILE_NAME);
$db   = pg_connect($conn_str);

// Insert binary to DB
$escaped_data = pg_escape_bytea($data);
pg_query("DELETE FROM ".$table_name." WHERE num = -9999;");
$sql = "INSERT INTO ".$table_name." (num, bin) VALUES (-9999, CAST ('".$escaped_data."' AS BYTEA));";
pg_query($db, $sql);

// Retrieve binary from DB
$sql = "SELECT bin::bytea FROM ".$table_name." WHERE num = -9999";
$result = pg_query($db, $sql);
$row = pg_fetch_array($result, 0, PGSQL_ASSOC);

if ($data === pg_unescape_bytea($row['bin'])) {
	echo "pg_escape_bytea() actually works with database\n";
}
else {
	echo "pg_escape_bytea() is broken\n";
}

// pg_escape_literal/pg_escape_identifier
$before = "ABC\\ABC\'";
$expect	 = " E'ABC\\\\ABC\\\\'''";
$after = pg_escape_literal($before);
if ($expect === $after) {
	echo "pg_escape_literal() is Ok\n";
}
else {
	echo "pg_escape_literal() is NOT Ok\n";
	var_dump($before);
	var_dump($after);
	var_dump($expect);
}

$before = "ABC\\ABC\'";
$expect	 = "\"ABC\ABC\'\"";
$after = pg_escape_identifier($before);
if ($expect === $after) {
	echo "pg_escape_identifier() is Ok\n";
}
else {
	echo "pg_escape_identifier() is NOT Ok\n";
	var_dump($before);
	var_dump($after);
	var_dump($expect);
}

?>
--EXPECT--
pg_escape_string() is Ok
pg_escape_bytea() is Ok
pg_escape_bytea() actually works with database
pg_escape_literal() is Ok
pg_escape_identifier() is Ok