--TEST--
GHSA-96wq-48vp-hh57: signed integer overflow of char array offset
--CREDITS--
012git012
--INI--
memory_limit=3G
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (PHP_INT_SIZE != 8) echo 'skip 64-bit only';
?>
--FILE--
<?php

$str = str_repeat('0', 2 * (1024 ** 3) - 2) . 'AE';
metaphone($str, 1);

?>
===DONE===
--EXPECT--
===DONE===
