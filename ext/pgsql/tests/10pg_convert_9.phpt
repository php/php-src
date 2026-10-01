--TEST--
PostgreSQL pg_convert() (9.0+)
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
$table_name = "table_10pg_convert_9";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");

pg_query($db, "SET standard_conforming_strings = 0");

$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');
$converted = pg_convert($db, $table_name, $fields);

var_dump($converted);

var_dump(pg_convert($db, $table_name, ['str' => "\\' OR 1=1"]));

/* Invalid values */
try {
    $converted = pg_convert($db, $table_name, [5 => 'AAA']);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_convert($db, $table_name, ['AAA']);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_convert($db, $table_name, ['num' => []]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_convert($db, $table_name, ['num' => new stdClass()]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $converted = pg_convert($db, $table_name, ['num' => $db]);
    var_dump($converted);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

/* standard_conforming_strings = 1 */
pg_query($db, "SET standard_conforming_strings = 1");
var_dump(pg_convert($db, $table_name, ['str' => "\\' OR 1=1"]));
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_10pg_convert_9";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
array(3) {
  [""num""]=>
  string(4) "1234"
  [""str""]=>
  string(5) "'AAA'"
  [""bin""]=>
  string(11) "'\\x424242'"
}
array(1) {
  [""str""]=>
  string(13) "'\\'' OR 1=1'"
}
Array of values must be an associative array with string keys
Array of values must be an associative array with string keys
Values must be of type string|int|float|bool|null, array given
Values must be of type string|int|float|bool|null, stdClass given
Values must be of type string|int|float|bool|null, PgSql\Connection given
array(1) {
  [""str""]=>
  string(12) "'\'' OR 1=1'"
}
