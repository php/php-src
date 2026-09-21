--TEST--
GH-22004: ZEND_FE_FETCH_R with key operand
--CREDITS--
YuanchengJiang
--FILE--
<?php

function doRandom($iter) {
    for ($i = 0; $i < $iter; $i++) {
        $lines = [];
        for ($j = 0; $j < $nLines; $j++) {
            $lines[] = $line;
        }
        foreach ($lines as $i => $line) {
        }
    }
}
$iter = 20;
doRandom($iter);

?>
==DONE==
--EXPECTF--
Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d

Warning: Undefined variable $nLines in %s on line %d
==DONE==
