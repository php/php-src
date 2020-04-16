--TEST--
Bug #67539 (ArrayIterator use-after-free due to object change during sorting)
--FILE--
<?php

$it = new ArrayIterator(array_fill(0,2,'X'), 1 );

function badsort($a, $b) {
        $GLOBALS['it']->unserialize($GLOBALS['it']->serialize());
        return TRUE;
}
try {
    $it->uksort('badsort');
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}
--EXPECT--
Modification of ArrayObject during sorting is prohibited
