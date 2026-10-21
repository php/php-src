--TEST--
json_decode() with JSON_INVALID_UTF8_SUBSTITUTE doesn't reach hard limit
--INI--
memory_limit=16M
--FILE--
<?php
$n = 10 * 1024 * 1024;
$json = str_repeat("\x80", $n);
$json[0] = '"';
$json[$n - 1] = '"';
json_decode($json, false, 512, JSON_INVALID_UTF8_SUBSTITUTE);
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
