--TEST--
Timeout does not execute finally blocks
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
max_execution_time=1
--FILE--
<?php

$gen1 = (function () {
    try {
        yield;
    } finally {
        echo "finally executed (1)\n";
    }
})();
$gen1->current();

$gen2 = (function () {
    try {
        while (true) {}
    } finally {
        echo "finally executed (2)\n";
    }
    yield;
})();

try {
    foreach ($gen2 as $value) {}
} finally {
    echo "finally executed (3)\n";
}

?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %stimeout_variation_13.php on line %d
