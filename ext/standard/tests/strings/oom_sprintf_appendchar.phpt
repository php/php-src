--TEST--
OOM sprintf() growing the buffer one character at a time
--INI--
memory_limit=72M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php

// Each %% adds one character. The output reaches the length of the buffer
// (240 bytes times 2^17), and the buffer doubles to about 60 MB.
$format = str_repeat("%%", 31457280);

echo "Setup done", PHP_EOL;

$result = sprintf($format);

echo "Not reached", PHP_EOL;
?>
--EXPECTF--
Setup done

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 9
