--TEST--
Bug #77047 pg_insert has a broken regex for the 'TIME WITHOUT TIMEZONE' data type
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = 'table_bug77047';

$db = pg_connect($conn_str);

pg_query($db, "CREATE TABLE {$table_name} (
        t TIME WITHOUT TIME ZONE
    )");

try {
    pg_insert($db, $table_name, array("t" => "13:31"));
} catch (\TypeError $e) {
    echo $e->getMessage();
}
pg_insert($db, $table_name, array("t" => "13:31:13"));
pg_insert($db, $table_name, array("t" => "1:2:3"));
try {
    pg_insert($db, $table_name, array("t" => "xyz"));
} catch (\TypeError $e) {
    echo $e->getMessage() . PHP_EOL;
}
pg_insert($db, $table_name, array("t" => NULL));
pg_insert($db, $table_name, array("t" => ""));

$res = pg_query($db, "SELECT t FROM {$table_name}");
while (false !== ($row = pg_fetch_row($res))) {
    var_dump(array_pop($row));
}

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$table_name = 'table_bug77047';

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECTF--
pg_insert(): Field "t" must be of type string|null, time given
string(8) "13:31:00"
string(8) "13:31:13"
string(8) "01:02:03"
NULL
NULL
