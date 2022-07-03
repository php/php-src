--TEST--
Bug #31720 (Invalid object callbacks not caught in array_walk())
--FILE--
<?php
$array = array('at least one element');

try {
    array_walk($array, array($nonesuchvar,'show'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $nonesuchvar (This will become an error in PHP 9.0) in %s on line %d
array_walk(): Argument #2 ($callback) must be a valid callback, first array member is not a valid class name or object
