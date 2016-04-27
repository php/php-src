--TEST--
Bug 72093: bcpowmod accepts negative scale and corrupts _one_ definition
--SKIPIF--
<?php
if(!extension_loaded("bcmath")) print "skip";
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip valid only for windows');
}
?>
--FILE--
<?php
var_dump(bcpowmod(1, "A", 128, -200));
var_dump(bcpowmod(1, 1.2, 1, 1));
?>
--EXPECTF--
string(1) "1"
string(3) "0.0"
bc math warning: non-zero scale in exponent
