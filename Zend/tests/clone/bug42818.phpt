--TEST--
Bug #42818 ($foo = clone(array()); leaks memory)
--FILE--
<?php
try {
    $foo = clone(array());
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: clone(): Argument #1 ($object) must be of type object, array given
