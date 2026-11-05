--TEST--
OOM sprintf() with an invalid specifier after the failed buffer growth
--INI--
memory_limit=40M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php

// The failed buffer growth for the literal text comes first. The invalid
// specifier after it must not replace the out of memory error.
$format = str_repeat("a", 31457281) . "%z";

echo "Setup done", PHP_EOL;

$result = sprintf($format);

echo "Not reached", PHP_EOL;
?>
--EXPECTF--
Setup done

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 9
