--TEST--
bzdecompress() rejects input larger than 4294967296
--EXTENSIONS--
bz2
--INI--
memory_limit=8G
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (PHP_INT_SIZE != 8) echo 'skip 64-bit only';
?>
--FILE--
<?php

try {
    $data = str_repeat("A", 4294967296);
    bzdecompress($data);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
bzdecompress(): Argument #1 ($data) must not exceed %d bytes
