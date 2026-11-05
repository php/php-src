--TEST--
OOM bin2hex()
--INI--
memory_limit=20M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php

// The string fits into the limit. The hex result is twice as long, and
// together they pass the hard limit.
$s = str_repeat("a", 15 * 1024 * 1024);

echo "Setup done", PHP_EOL;

$result = bin2hex($s);

echo "Not reached", PHP_EOL;
?>
--EXPECTF--
Setup done

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 9
