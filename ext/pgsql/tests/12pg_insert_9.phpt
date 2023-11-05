--TEST--
PostgreSQL pg_insert() (9.0+)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
skip_bytea_not_hex();
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = "table_12pg_insert_9";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");

pg_query($db, "SET standard_conforming_strings = 0");

$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');

pg_insert($db, $table_name, $fields) or print "Error in test 1\n";
echo pg_insert($db, $table_name, $fields, PGSQL_DML_STRING)."\n";
echo pg_insert($db, $table_name, $fields, PGSQL_DML_STRING|PGSQL_DML_ESCAPE)."\n";
var_dump( pg_insert($db, $table_name, $fields, PGSQL_DML_EXEC) );

/* Invalid values */
try {
    $converted = pg_insert($db, $table_name, [5 => 'AAA']);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_insert($db, $table_name, ['AAA']);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_insert($db, $table_name, ['num' => []]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_insert($db, $table_name, ['num' => new stdClass()]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_insert($db, $table_name, ['num' => $db]);
    var_dump($converted);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "Ok\n";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_12pg_insert_9";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE {$table_name}");
?>
--EXPECTF--
INSERT INTO "table_12pg_insert_9" ("num","str","bin") VALUES (1234,E'AAA',E'\\x424242');
INSERT INTO "table_12pg_insert_9" ("num","str","bin") VALUES ('1234','AAA','BBB');
object(PgSql\Result)#%d (0) {
}
Array of values must be an associative array with string keys
Array of values must be an associative array with string keys
Values must be of type string|int|float|bool|null, array given
Values must be of type string|int|float|bool|null, stdClass given
Values must be of type string|int|float|bool|null, PgSql\Connection given
Ok
