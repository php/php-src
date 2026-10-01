--TEST--
GH-15918 (Assertion failure in ext/spl/spl_fixedarray.c)
--FILE--
<?php
$foo = new SplFixedArray(5);
try {
    $arrayObject = new ArrayObject($foo);
} catch (InvalidArgumentException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
InvalidArgumentException: Overloaded object of type SplFixedArray is not compatible with ArrayObject
