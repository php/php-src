--TEST--
Bug #46160 (SPL - Memory leak when exception is throwed in offsetSet method)
--FILE--
<?php

try {
    $x = new splqueue;
    $x->offsetSet(0, 0);
} catch (Exception $e) { }

?>
DONE
--EXPECT--
DONE
