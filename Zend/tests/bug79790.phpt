--TEST--
Bug #79790: "Illegal offset type" exception during AST evaluation not handled properly
--FILE--
<?php
b();
function b($a = array()[array ()]) {
    ++$c[function () {}];
}
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot access offset of type array on array in %s:%d
Stack trace:
#0 %s(%d): b()
#1 {main}
  thrown in %s on line %d
