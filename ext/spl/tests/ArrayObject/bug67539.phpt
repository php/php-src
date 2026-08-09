--TEST--
Bug #67539 (ArrayIterator use-after-free due to object change during sorting)
--FILE--
<?php

$it = new ArrayIterator(array_fill(0,2,'X'), 1 );

function badsort($a, $b) {
    try {
        $GLOBALS['it']->unserialize($GLOBALS['it']->serialize());
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    return 0;
}

$it->uksort('badsort');
?>
--EXPECTF--
Deprecated: Method ArrayIterator::uksort() is deprecated since 8.6 in %s on line %d

Deprecated: Method ArrayIterator::serialize() is deprecated since 8.6 in %s on line %d

Deprecated: Method ArrayIterator::unserialize() is deprecated since 8.6 in %s on line %d
Error: Modification of ArrayObject during sorting is prohibited
