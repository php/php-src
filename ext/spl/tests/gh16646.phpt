--TEST--
GH-16646: Use-after-free in ArrayObject::unset() with destructor
--FILE--
<?php

class B {
    public $b;
    function __construct($arg) {
        $this->b = $arg;
    }
}

class C {
    function __destruct() {
        global $arr;
        echo __METHOD__, "\n";
        $arr->exchangeArray([]);
    }
}

$arr = new ArrayObject(new B(new C));
unset($arr["b"]);
var_dump($arr);

?>
--EXPECT--
C::__destruct
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
