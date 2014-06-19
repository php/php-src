--TEST--
PostgreSQL pg_convert()
--SKIPIF--
<?php
include("skipif.inc");
skip_bytea_not_escape();
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');
$converted = pg_convert($db, $table_name, $fields);

var_dump($converted);
?>
--EXPECT--
array(3) {
  [""num""]=>
  string(4) "1234"
  [""str""]=>
  string(6) "E'AAA'"
  [""bin""]=>
  string(6) "E'BBB'"
}