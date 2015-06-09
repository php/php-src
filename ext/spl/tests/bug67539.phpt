--TEST--
Bug #67539 (ArrayIterator use-after-free due to object change during sorting)
--FILE--
<?php

$it = new ArrayIterator(array_fill(0,2,'X'), 1 );

function badsort($a, $b) {
        $GLOBALS['it']->unserialize($GLOBALS['it']->serialize());
        return TRUE;
}

$it->uksort('badsort');
--EXPECTF--
Warning: Modification of ArrayObject during sorting is prohibited in %sbug67539.php on line %d
