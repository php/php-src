--TEST--
callable type#001
--FILE--
<?php

class bar {
    function baz() {}
    static function foo() {}
}
function foo(callable $bar) {
    var_dump($bar);
}
$closure = function () {};

foo("strpos");
foo("foo");
foo($closure);
foo(array("bar", "foo"));
foo(array("bar", "baz"));
?>
--EXPECTF--
string(6) "strpos"
string(3) "foo"
object(Closure)#%d (1) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
}
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(3) "foo"
}

Fatal error: Uncaught TypeError: foo(): Argument #1 ($bar) must be of type callable, array given, called in %s:%d
Stack trace:
#0 %s(%d): foo(Array)
#1 {main}
  thrown in %s on line %d
