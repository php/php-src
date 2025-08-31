--TEST--
bzread() tests
--EXTENSIONS--
bz2
--FILE--
<?php

$fd = bzopen(__DIR__."/003.txt.bz2","r");
var_dump(bzread($fd, 0));

try {
    var_dump(bzread($fd, -10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(bzread($fd, 1));
var_dump(bzread($fd, 2));
var_dump(bzread($fd, 100000));

?>
--EXPECT--
string(0) ""
bzread(): Argument #2 ($length) must be greater than or equal to 0
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
