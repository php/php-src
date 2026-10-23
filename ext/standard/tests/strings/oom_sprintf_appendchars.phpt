--TEST--
OOM sprintf() growing the buffer for literal text
--INI--
memory_limit=40M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php

// The format is longer than the 240 byte start buffer times 2^17, so the buffer
// grows to about 60 MB in one step.
$format = str_repeat("a", 31457281);

echo "Setup done", PHP_EOL;

$result = sprintf($format);

echo "Not reached", PHP_EOL;
?>
--EXPECTF--
Setup done

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 9
