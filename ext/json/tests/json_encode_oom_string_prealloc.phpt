--TEST--
json_encode() string pre-allocation doesn't reach hard limit
--INI--
memory_limit=32M
memory_hard_limit_percent=110
--FILE--
<?php
// The hard limit is lowered to 110% to keep the test cheap
$subject = str_repeat("a", 20 * 1024 * 1024);
json_encode($subject);
?>
--EXPECTF--
Fatal error: Allowed memory size of 33554432 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
