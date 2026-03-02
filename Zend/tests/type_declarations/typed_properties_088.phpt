--TEST--
Check for correct invalidation of prop_info cache slots
--FILE--
<?php

class A {
    public int $prop;
}
class B {
    public $prop;
}

function test($obj) {
    $obj->prop = "42";
    var_dump($obj);
}

test(new A);
test(new B);

?>
--EXPECT--
object(A)#1 (1) {
  ["prop"]=>
  int(42)
}
object(B)#1 (1) {
  ["prop"]=>
  string(2) "42"
}
