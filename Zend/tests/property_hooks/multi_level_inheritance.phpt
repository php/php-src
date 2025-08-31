--TEST--
Property hooks with multi level inheritance
--FILE--
<?php

class A {
    public $prop = 1;
}

class B extends A {
    public $prop = 2 { get => parent::$prop::get() * 2; }
}

class C extends B {
    public $prop = 3;
}

function test(A $a) {
    var_dump($a);
    var_dump((array)$a);
    var_dump(unserialize(serialize($a)));
    var_dump(get_object_vars($a));
    var_dump(json_decode(json_encode($a)));
}

test(new B);
test(new C);

?>
--EXPECTF--
object(B)#%d (1) {
  ["prop"]=>
  int(2)
}
array(1) {
  ["prop"]=>
  int(2)
}
object(B)#%d (1) {
  ["prop"]=>
  int(2)
}
array(1) {
  ["prop"]=>
  int(4)
}
object(stdClass)#%d (1) {
  ["prop"]=>
  int(4)
}
object(C)#%d (1) {
  ["prop"]=>
  int(3)
}
array(1) {
  ["prop"]=>
  int(3)
}
object(C)#%d (1) {
  ["prop"]=>
  int(3)
}
array(1) {
  ["prop"]=>
  int(6)
}
object(stdClass)#%d (1) {
  ["prop"]=>
  int(6)
}
