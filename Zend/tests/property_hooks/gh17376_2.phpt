--TEST--
GH-17376: Mutli-level inheritance with property hooks
--FILE--
<?php

class A {
    public $prop = 1;
}

class B extends A {
    public $prop = 1 { get => parent::$prop::get() * 2; }
}

class C extends B {
    public $prop = 3;
}

function test(A $a) {
    var_dump((array)$a);
}

test(new C);

?>
--EXPECT--
array(1) {
  ["prop"]=>
  int(3)
}
