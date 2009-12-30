--TEST--
PostgreSQL pg_convert()
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

$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');
$converted = pg_convert($db, $table_name, $fields);

var_dump($converted);
?>
--EXPECT--
array(3) {
  [u"num"]=>
  unicode(4) "1234"
  [u"str"]=>
  unicode(5) "'AAA'"
  [u"bin"]=>
  unicode(5) "'BBB'"
}
