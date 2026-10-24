--TEST--
OOM vprintf() with a huge field width
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php

echo "Setup done", PHP_EOL;

$result = vprintf("%1000000000d", [1]);

echo "Not reached", PHP_EOL;
?>
--EXPECTF--
Setup done

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 5
