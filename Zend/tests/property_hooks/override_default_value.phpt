--TEST--
Property with hook may override default value
--FILE--
<?php

class A {
    public $prop = 41;
}

class B extends A {
    public $prop = 42 {
        get {
            return 43;
        }
    }
}

$b = new B();
var_dump($b);
var_dump($b->prop);
$b->prop = 44;
var_dump($b);
var_dump($b->prop);

?>
--EXPECT--
object(B)#1 (1) {
  ["prop"]=>
  int(42)
}
int(43)
object(B)#1 (1) {
  ["prop"]=>
  int(44)
}
int(43)
