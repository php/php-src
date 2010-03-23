--TEST--
PostgreSQL pg_convert() (8.5+)
--SKIPIF--
<?php
include("skipif.inc");
skip_server_version('8.5dev', '<');
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
  ["num"]=>
  string(4) "1234"
  ["str"]=>
  string(5) "'AAA'"
  ["bin"]=>
  string(11) "'\\x424242'"
}
