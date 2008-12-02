--TEST--
Double precision is used for floating point calculations
--FILE--
<?php
var_dump (0.002877 == 2877.0 / 1000000.0);
var_dump (substr (sprintf ("%.35f", 0.002877), 0, 10));
?>
--EXPECT--
bool(true)
string(10) "0.00287699"
