--TEST--
Bug 72093: bcpowmod accepts negative scale and corrupts _one_ definition
--SKIPIF--
<?php
if(!extension_loaded("bcmath")) print "skip";
?>
--FILE--
<?php
var_dump(bcpowmod(1, "A", 128, -200));
var_dump(bcpowmod(1, 1.2, 1, 1));
?>
--EXPECTF--
string(1) "1"

Warning: bcpowmod(): non-zero scale in exponent in %s on line %d
string(3) "0.0"
