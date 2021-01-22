--TEST--
Bug #65251: array_merge_recursive() recursion detection broken
--FILE--
<?php

/* This no longer involves any recursion. */
try {
    array_merge_recursive($GLOBALS, $GLOBALS);
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
===DONE===
--EXPECT--
===DONE===
