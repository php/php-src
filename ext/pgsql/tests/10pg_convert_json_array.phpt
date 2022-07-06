--TEST--
PostgreSQL pg_convert() and JSON/Array
--SKIPIF--
<?php
include("skipif.inc");
skip_server_version('9.2');
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$fields = array(
    'textary'=>'{"meeting", "lunch", "training", "presentation"}',
    'jsn'=>'{"f1":1,"f2":"foo"}',
);
$converted = pg_convert($db, $table_name_92, $fields);
var_dump($converted);

if (!pg_insert($db, $table_name_92, $fields)) {
    echo "Error\n";
} else {
    echo "OK\n";
}

?>
--EXPECT--
array(2) {
  [""textary""]=>
  string(51) "E'{"meeting", "lunch", "training", "presentation"}'"
  [""jsn""]=>
  string(22) "E'{"f1":1,"f2":"foo"}'"
}
OK
