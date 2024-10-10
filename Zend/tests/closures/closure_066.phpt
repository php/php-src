--TEST--
Dumping closures includes the name.
--FILE--
<?php
var_dump(function($foo) { });
?>
--EXPECTF--
object(Closure)#1 (4) {
  ["name"]=>
  string(%d) "{closure:%s:2}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
  ["parameter"]=>
  array(1) {
    ["$foo"]=>
    string(10) "<required>"
  }
}
