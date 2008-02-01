--TEST--
Test base_convert() - wrong params base_convert()
--FILE--
<?php
base_convert();
base_convert(35);
base_convert(35,2);
base_convert(1234, 1, 10);
base_convert(1234, 10, 37);
?>
--EXPECTF--

Warning: base_convert() expects exactly 3 parameters, 0 given in %s on line %d

Warning: base_convert() expects exactly 3 parameters, 1 given in %s on line %d

Warning: base_convert() expects exactly 3 parameters, 2 given in %s on line %d

Warning: base_convert(): Invalid `from base' (1) in %s on line %d

Warning: base_convert(): Invalid `to base' (37) in %s on line %d
