--TEST--
bzread() tests
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php

$fd = bzopen(__DIR__."/003.txt.bz2","r");
var_dump(bzread());
var_dump(bzread($fd, 1 ,0));
var_dump(bzread($fd, 0));
var_dump(bzread($fd, -10));
var_dump(bzread($fd, 1));
var_dump(bzread($fd, 2));
var_dump(bzread($fd, 100000));

echo "Done\n";
?>
--EXPECTF--
Warning: bzread() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: bzread() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
string(0) ""

Warning: bzread(): length may not be negative in %s on line %d
bool(false)
string(1) "R"
string(2) "is"
string(251) "ing up from the heart of the desert
Rising up for Jerusalem
Rising up from the heat of the desert
Building up Old Jerusalem
Rising up from the heart of the desert
Rising up for Jerusalem
Rising up from the heat of the desert
Heading out for Jerusalem
"
Done
