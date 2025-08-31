--TEST--
Bug #44995 (bcpowmod() fails if scale != 0)
--EXTENSIONS--
bcmath
--FILE--
<?php
var_dump(bcpowmod('4', '4', '3', 1));
var_dump(bcpowmod('3234', '32345', '22345', 1));
?>
--EXPECT--
string(3) "1.0"
string(7) "17334.0"
