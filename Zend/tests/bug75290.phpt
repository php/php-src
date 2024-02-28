--TEST--
Bug #75290 (debug info of Closures of internal functions contain garbage argument names)
--FILE--
<?php

var_dump((new ReflectionFunction('sin'))->getClosure());

var_dump(function ($someThing) {});

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(3) "sin"
  ["parameter"]=>
  array(1) {
    ["$num"]=>
    string(10) "<required>"
  }
}
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["parameter"]=>
  array(1) {
    ["$someThing"]=>
    string(10) "<required>"
  }
}
