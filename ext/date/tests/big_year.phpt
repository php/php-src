--TEST--
Handling of large year values
--SKIPIF--
<?php echo PHP_INT_SIZE != 8 ? "skip 64-bit only" : "OK"; ?>
--FILE--
<?php
date_default_timezone_set("America/Toronto");

$t = mktime(0,0,0,1,1,292277026596);

var_dump(date("r", $t));

echo "OK\n";
?>
--EXPECT--
string(39) "Fri, 01 Jan 292277026596 00:00:00 -0500"
OK
