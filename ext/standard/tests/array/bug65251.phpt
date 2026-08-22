--TEST--
Bug #65251: array_merge_recursive() recursion detection broken
--FILE--
<?php

/* This no longer involves any recursion. */
try {
    array_merge_recursive($GLOBALS, $GLOBALS);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECT--
===DONE===
