--TEST--
Array type declaration
--FILE--
<?php
function foo(array $a) {
    echo count($a)."\n";
}

foo(array(1,2,3));
foo(123);
?>
--EXPECTF--
3

Fatal error: Uncaught TypeError: foo() expects argument #1 ($a) to be of type array, int given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(123)
#1 {main}
  thrown in %s on line 2
