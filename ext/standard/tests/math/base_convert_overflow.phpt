--TEST--
Test base_convert() - overflow test
--FILE--
<?php
var_dump(base_convert('fffffffffffffffff', 16, 15));
?>
--EXPECTF--

Notice: Input number exceeds maximum integer value, precision has been lost in conversion in %s on line %d
string(18) "2ee03c32ad644bd1e1"