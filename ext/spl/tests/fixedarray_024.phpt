--TEST--
SPL: FixedArray: Bug GH-8041 (php 8.2.0-dev crashes with assertion for cloning/get_object_vars on non-empty SplFixedArray)
--FILE--
<?php
$x = new SplFixedArray(1);
$z = (array)$x;
$y = clone $x;
?>
DONE
--EXPECT--
DONE
