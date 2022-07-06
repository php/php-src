--TEST--
Bug #67539 (ArrayIterator use-after-free due to object change during sorting)
--FILE--
<?php

$it = new ArrayIterator(array_fill(0,2,'X'), 1 );

function badsort($a, $b) {
    try {
        $GLOBALS['it']->unserialize($GLOBALS['it']->serialize());
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    return 0;
}

$it->uksort('badsort');
?>
--EXPECT--
Modification of ArrayObject during sorting is prohibited
