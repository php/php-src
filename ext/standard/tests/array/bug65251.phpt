--TEST--
Bug #65251: array_merge_recursive() recursion detection broken
--FILE--
<?php

try {
    array_merge_recursive($GLOBALS, $GLOBALS);
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Recursion detected
