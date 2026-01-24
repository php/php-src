--TEST--
PFA variation: named may overwrite variadic placeholder
--FILE--
<?php
function foo($a) {
    var_dump(func_get_args());
}

$foo = foo(a: "a", ...);

$foo();
?>
--EXPECTF--
array(1) {
  [0]=>
  string(1) "a"
}
