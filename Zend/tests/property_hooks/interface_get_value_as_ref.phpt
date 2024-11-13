--TEST--
By-value get may be implemented as by-reference
--FILE--
<?php

interface I {
    public $prop { get; }
}

class A implements I {
    private $_prop;
    public $prop {
        &get => $this->_prop;
    }
}

function test(I $i) {
    $ref = &$i->prop;
    $ref = 42;
}

$a = new A();
test($a);
var_dump($a);

?>
--EXPECT--
object(A)#1 (1) {
  ["_prop":"A":private]=>
  int(42)
}
