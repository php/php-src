--TEST--
PostgreSQL pg_select()
--SKIPIF--
<?php
include("skipif.inc");
skip_server_version('8.5dev', '>=');
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);
$fields = array('num'=>'1234', 'str'=>'ABC', 'bin'=>'XYZ');
$ids = array('num'=>'1234');

$res = pg_select($db, $table_name, $ids) or print "Error\n";
var_dump($res);
echo pg_select($db, $table_name, $ids, PGSQL_DML_STRING)."\n";
echo "Ok\n";

?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    ["num"]=>
    string(4) "1234"
    ["str"]=>
    string(3) "AAA"
    ["bin"]=>
    string(3) "BBB"
  }
}
SELECT * FROM "php_pgsql_test" WHERE "num"=1234;
Ok
