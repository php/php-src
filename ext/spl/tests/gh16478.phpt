--TEST--
GH-16478: Use-after-free in SplFixedArray::unset()
--FILE--
<?php

class C {
    function __destruct() {
        global $arr;
        $arr->setSize(0);
    }
}

$arr = new SplFixedArray(2);
$arr[0] = new C;
unset($arr[0]);
var_dump($arr);

?>
--EXPECT--
object(SplFixedArray)#1 (0) {
}
