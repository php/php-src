--TEST--
OSS-Fuzz #540904105 (ASSERT: ast->attr == T_CLASS_C)
--FILE--
<?php

function foo($test, $extra) {}

const C = foo(__METHOD__, ?);

var_dump(C);

?>
--EXPECTF--
object(Closure)#%d (%d) {
  ["name"]=>
  string(%d) "%s"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(5)
  ["static"]=>
  array(1) {
    ["test"]=>
    string(0) ""
  }
  ["parameter"]=>
  array(1) {
    ["$extra"]=>
    string(10) "<required>"
  }
}
