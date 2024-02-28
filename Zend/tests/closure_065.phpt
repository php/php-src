--TEST--
Dumping closures includes the name.
--FILE--
<?php
var_dump(function($foo) { });
?>
--EXPECTF--
object(Closure)#1 (2) {
  ["name"]=>
  string(%d) "{closure:%s:2}"
  ["parameter"]=>
  array(1) {
    ["$foo"]=>
    string(10) "<required>"
  }
}
