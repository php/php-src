--TEST--
Handling of large year values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--FILE--
<?php
date_default_timezone_set("America/Toronto");

$t = mktime(0,0,0,1,1,2922770265);

var_dump(date("r", $t));

echo "OK\n";
?>
--EXPECTF--
string(%d) "%s, 01 Jan 2922770265 00:00:00 -0500"
OK
