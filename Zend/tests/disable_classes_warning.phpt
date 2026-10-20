--TEST--
Check that warning is emitted when disabling classes
--INI--
disable_classes=Exception
--FILE--
<?php
$o = new Exception();
var_dump($o);
?>
--EXPECTF--
object(Exception)#1 (7) {
  ["message":protected]=>
  string(0) ""
  ["string":"Exception":private]=>
  string(0) ""
  ["code":protected]=>
  int(0)
  ["file":protected]=>
  string(%d) "%s"
  ["line":protected]=>
  int(2)
  ["trace":"Exception":private]=>
  array(0) {
  }
  ["previous":"Exception":private]=>
  NULL
}
