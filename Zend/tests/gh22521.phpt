--TEST--
GH-22521: Infinite recursion inside try/catch should not crash unpredictably
--INI--
memory_limit=8G
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
try {
    function foo() {
        foo();
    }
    foo();
} catch (\Throwable $_ffl_e) {
    echo "Caught throwable\n";
}
echo "End\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes) in %s on line %d
