--TEST--
Bug 72093: bcpowmod accepts negative scale and corrupts _one_ definition
--SKIPIF--
<?php
if(!extension_loaded("bcmath")) print "skip";
if (substr(PHP_OS, 0, 3) == 'WIN') {
	$cur = PHP_WINDOWS_VERSION_MAJOR.'.'.PHP_WINDOWS_VERSION_MINOR.'.'.PHP_WINDOWS_VERSION_BUILD;
	$req = "10.0.17134";
	if (version_compare($cur, $req) < 0) {
		echo "skip Compatible on Windows systems >= $req";
	}
}
?>
--FILE--
<?php
var_dump(bcpowmod(1, "A", 128, -200));
var_dump(bcpowmod(1, 1.2, 1, 1));
?>
--EXPECTF--
string(1) "1"
bc math warning: non-zero scale in exponent
string(3) "0.0"
