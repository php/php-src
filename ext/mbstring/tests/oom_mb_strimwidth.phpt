--TEST--
mb_strimwidth() doesn't reach hard limit - trim marker
--EXTENSIONS--
mbstring
--INI--
memory_limit=16M
--FILE--
<?php

$string = str_repeat("a", 6 * 1024 * 1024);
$marker = str_repeat("b", 5 * 1024 * 1024);

mb_strimwidth($string, 0, 5 * 1024 * 1024 + 512 * 1024, $marker);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
