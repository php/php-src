--TEST--
json_encode() with JSON_PRETTY_PRINT doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php
$subject = array_fill(0, 200000, [1, 2, 3]);
json_encode($subject, JSON_PRETTY_PRINT);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
