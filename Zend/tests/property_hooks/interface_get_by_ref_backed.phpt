--TEST--
Backed prop satisfies interface get hook by-reference
--FILE--
<?php

interface I {
    public $prop { get; }
}

class A implements I {
    public $prop = 42 {
        get => $this->prop;
    }
}

$a = new A();
var_dump($a);

?>
--EXPECT--
object(A)#1 (1) {
  ["prop"]=>
  int(42)
}
