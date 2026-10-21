--TEST--
json_encode() escaping doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php
$subject = str_repeat("\x01", 2 * 1024 * 1024);
json_encode($subject);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
