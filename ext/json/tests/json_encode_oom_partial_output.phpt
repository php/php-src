--TEST--
json_encode() with JSON_PARTIAL_OUTPUT_ON_ERROR doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php
$subject = array_fill(0, 200, str_repeat("\x01", 20000));
json_encode($subject, JSON_PARTIAL_OUTPUT_ON_ERROR);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
