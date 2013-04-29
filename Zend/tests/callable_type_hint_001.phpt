--TEST--
callable type hint#001
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
foo(array("bar", "baz"));
foo(array("bar", "foo"));
foo($closure);
--EXPECTF--
string(6) "strpos"
string(3) "foo"

Strict Standards: Non-static method bar::baz() should not be called statically in %scallable_type_hint_001.php on line %d
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(3) "baz"
}
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(3) "foo"
}
object(Closure)#%d (0) {
}

