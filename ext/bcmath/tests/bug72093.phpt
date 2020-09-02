--TEST--
Bug 72093: bcpowmod fails on negative scale and corrupts _one_ definition
--SKIPIF--
<?php
if(!extension_loaded("bcmath")) print "skip";
?>
--FILE--
<?php
try {
    var_dump(bcpowmod(1, 0, 128, -200));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(bcpowmod(1, 1.2, 1, 1));
?>
--EXPECTF--
bcpowmod(): Argument #4 ($scale) must be between 0 and 2147483647

Warning: bcpowmod(): Non-zero scale in exponent in %s on line %d
string(3) "0.0"
