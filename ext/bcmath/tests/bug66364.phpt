--TEST--
Bug #66364 (BCMath bcmul ignores scale parameter)
--EXTENSIONS--
bcmath
--FILE--
<?php
var_dump(bcmul('0.3', '0.2', 4));
?>
--EXPECT--
string(6) "0.0600"
