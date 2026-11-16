--TEST--
preg_split() doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php
$subject = str_repeat("ab", 1024 * 1024);
preg_split('/a/', $subject);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
