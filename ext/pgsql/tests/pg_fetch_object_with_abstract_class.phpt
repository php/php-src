--TEST--
pg_fetch_object() with abstract class name
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php

interface I {}

abstract class C {}

enum E {
	case A;
}

include "inc/config.inc";
$table_name = "pg_fetch_object_abstract_class";
$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (a integer, b text)");
pg_query($db, "INSERT INTO {$table_name} VALUES(0, 'ABC')");

$sql = "SELECT * FROM $table_name WHERE a = 0";

try {
	$result = pg_query($db, $sql);
	var_dump(pg_fetch_object($result, NULL, 'I'));
} catch(Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$result = pg_query($db, $sql);
	var_dump(pg_fetch_object($result, NULL, 'C'));
} catch(Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$result = pg_query($db, $sql);
	var_dump(pg_fetch_object($result, NULL, 'E'));
} catch(Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = @pg_connect($conn_str);
@pg_query($db, "DROP TABLE IF EXISTS pg_fetch_object_abstract_class cascade");
?>
--EXPECT--
Error: Cannot instantiate interface I
Error: Cannot instantiate abstract class C
Error: Cannot instantiate enum E
