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
Warning: Undefined variable $x in %s on line %d
