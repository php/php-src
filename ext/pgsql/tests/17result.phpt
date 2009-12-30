--TEST--
PostgreSQL pg_fetch_*() functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$sql = "SELECT * FROM $table_name";
$result = pg_query($db, $sql) or die('Cannot qeury db');
$rows = pg_num_rows($result);

var_dump(pg_result_seek($result, 1));
var_dump(pg_fetch_object($result));
var_dump(pg_fetch_array($result, 1));
var_dump(pg_fetch_row($result, 1));
var_dump(pg_fetch_assoc($result, 1));
var_dump(pg_result_seek($result, 0));

echo "Ok\n";
?>
--EXPECT--
bool(true)
object(stdClass)#1 (3) {
  [u"num"]=>
  unicode(1) "1"
  [u"str"]=>
  unicode(3) "ABC"
  [u"bin"]=>
  NULL
}
array(6) {
  [0]=>
  unicode(1) "1"
  [u"num"]=>
  unicode(1) "1"
  [1]=>
  unicode(3) "ABC"
  [u"str"]=>
  unicode(3) "ABC"
  [2]=>
  NULL
  [u"bin"]=>
  NULL
}
array(3) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(3) "ABC"
  [2]=>
  NULL
}
array(3) {
  [u"num"]=>
  unicode(1) "1"
  [u"str"]=>
  unicode(3) "ABC"
  [u"bin"]=>
  NULL
}
bool(true)
Ok
