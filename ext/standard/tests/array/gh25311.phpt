--TEST--
GH-25311 (Assertion ret == SUCCESS in array_map()/array_filter() after delayed out-of-memory)
--INI--
memory_limit=10M
--FILE--
<?php

$s = str_repeat("a", 5 * 1024 * 1024);
$s .= $s;
array_map(fn($x) => $x, [1]);

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %s
