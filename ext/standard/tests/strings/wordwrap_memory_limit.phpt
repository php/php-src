--TEST--
No overflow should occur during the memory_limit check for wordwrap()
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) die("skip this test is not for 32bit platforms");
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--INI--
memory_limit=128M
--FILE--
<?php

$str = str_repeat('x', 65534);
$str2 = str_repeat('x', 65535);
wordwrap($str, 1, $str2);

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted by %d bytes%S. Allocated %d bytes and need to allocate %d bytes in %s on line %d
