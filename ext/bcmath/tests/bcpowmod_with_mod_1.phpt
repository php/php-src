--TEST--
bcpowmod() must return 0 when mod is +1 or -1
--EXTENSIONS--
bcmath
--FILE--
<?php
var_dump(bcpowmod(5, 0, 1));
var_dump(bcpowmod(5, 0, 1, 3));
var_dump(bcpowmod(5, 0, -1));
var_dump(bcpowmod(5, 0, -1, 3));
?>
--EXPECT--
string(1) "0"
string(5) "0.000"
string(1) "0"
string(5) "0.000"
