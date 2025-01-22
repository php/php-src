--TEST--
GH-16646: Use-after-free in ArrayObject::exchangeArray() with destructor
--FILE--
<?php

class C {
    function __destruct() {
        global $arr;
        echo __METHOD__, "\n";
        $arr->exchangeArray([]);
    }
}

$arr = new ArrayObject(new C);
$arr->exchangeArray([]);
var_dump($arr);

?>
--EXPECT--
C::__destruct
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
