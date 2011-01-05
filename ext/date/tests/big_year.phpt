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
string(36) "Fri, 01 Jan 219250468 00:00:00 -0500"
OK
