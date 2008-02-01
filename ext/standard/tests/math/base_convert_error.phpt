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

Warning: Wrong parameter count for base_convert() in %s on line 2

Warning: Wrong parameter count for base_convert() in %s on line 3

Warning: Wrong parameter count for base_convert() in %s on line 4

Warning: base_convert(): Invalid `from base' (1) in %s on line 5

Warning: base_convert(): Invalid `to base' (37) in %s on line 6
