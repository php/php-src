--TEST--
No overflow should occur during the memory_limit check for wordwrap()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
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
Fatal error: %rAllowed memory size of %d bytes exhausted%s\(tried to allocate %d bytes\)|Possible integer overflow in memory allocation \(4294901777 \+ %d\)%r in %s on line %d
