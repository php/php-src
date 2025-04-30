--TEST--
GH-15918 (Assertion failure in ext/spl/spl_fixedarray.c)
--FILE--
<?php
$foo = new SplFixedArray(5);
try {
    $arrayObject = new ArrayObject($foo);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Overloaded object of type SplFixedArray is not compatible with ArrayObject
