--TEST--
Bug #42817 (clone() on a non-object does not result in a fatal error)
--FILE--
<?php
try {
    $a = clone(null);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: clone(): Argument #1 ($object) must be of type object, null given
