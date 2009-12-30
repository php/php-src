--TEST--
PostgreSQL pg_select() (8.5+)
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
    [u"num"]=>
    unicode(4) "1234"
    [u"str"]=>
    unicode(3) "AAA"
    [u"bin"]=>
    unicode(8) "\x424242"
  }
}
SELECT * FROM php_pgsql_test WHERE num=1234;
Ok
