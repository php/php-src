--TEST--
No overflow should occur during the memory_limit check for wordwrap()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN' && PHP_INT_SIZE == 4) die("skip this test is not for 32bit Windows platforms");
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
Fatal error: Allowed memory size of 134217728 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
