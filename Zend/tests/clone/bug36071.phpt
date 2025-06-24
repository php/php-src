--TEST--
Bug #36071 (Engine Crash related with 'clone')
--INI--
error_reporting=4095
--FILE--
<?php
try {
    $a = clone 0;
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: clone(): Argument #1 ($object) must be of type object, int given
