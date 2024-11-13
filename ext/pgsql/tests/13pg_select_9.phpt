--TEST--
PostgreSQL pg_select() (9.0+)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
skip_server_version('9.0', '<');
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = "table_13pg_select_9";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");
pg_query($db, "INSERT INTO {$table_name} VALUES(1234, 'AAA', 'BBB')");
pg_query($db, "INSERT INTO {$table_name} VALUES(1234, 'AAA', 'BBB')");

pg_query($db, "SET bytea_output = 'hex'");

$ids = array('num'=>'1234');

$res = pg_select($db, $table_name, $ids) or print "Error\n";
var_dump($res);
echo pg_select($db, $table_name, $ids, PGSQL_DML_STRING)."\n";
echo pg_select($db, $table_name, $ids, PGSQL_DML_STRING|PGSQL_DML_ESCAPE)."\n";

/* Invalid values */
try {
    $converted = pg_select($db, $table_name, [5 => 'AAA']);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_select($db, $table_name, ['AAA']);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_select($db, $table_name, ['num' => []]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_select($db, $table_name, ['num' => new stdClass()]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_select($db, $table_name, ['num' => $db]);
    var_dump($converted);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "Ok\n";

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_13pg_select_9";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
array(2) {
  [0]=>
  array(3) {
    ["num"]=>
    string(4) "1234"
    ["str"]=>
    string(3) "AAA"
    ["bin"]=>
    string(8) "\x424242"
  }
  [1]=>
  array(3) {
    ["num"]=>
    string(4) "1234"
    ["str"]=>
    string(3) "AAA"
    ["bin"]=>
    string(8) "\x424242"
  }
}
SELECT * FROM "table_13pg_select_9" WHERE "num"=1234;
SELECT * FROM "table_13pg_select_9" WHERE "num"='1234';
Array of values must be an associative array with string keys
Array of values must be an associative array with string keys
Values must be of type string|int|float|bool|null, array given
Values must be of type string|int|float|bool|null, stdClass given
Values must be of type string|int|float|bool|null, PgSql\Connection given
Ok
