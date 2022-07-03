--TEST--
Code before loop var free
--FILE--
<?php
switch ($x > 0) {
default:
    return;
    Y;
}
?>
--EXPECTF--
Warning: Undefined variable $x (This will become an error in PHP 9.0) in %s on line %d
