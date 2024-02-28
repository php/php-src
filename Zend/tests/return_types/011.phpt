--TEST--
Function returned callable, expected callable
--FILE--
<?php
function foo() : callable {
    return function() {};
}

var_dump(foo());
?>
--EXPECTF--
object(Closure)#%d (%d) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
}
