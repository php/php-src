--TEST--
Recursion should not crash in zend_fetch_debug_backtrace
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) {
    die('skip slow test');
}
?>
--INI--
memory_limit=30G
--FILE--
<?php

function foo() {
    foo();
}

try {
    foo();
} catch (Throwable $e) {
    echo "Caught exception\n";
}

echo "DONE\n";

?>
--EXPECTF--
Caught exception
DONE