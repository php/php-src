--TEST--
preg_match() with overlapping captures doesn't reach hard limit
--INI--
memory_limit=128M
--FILE--
<?php
$subject = str_repeat('a', 6 * 1024 * 1024);
$pattern = '/^' . str_repeat('(', 48) . '.*' . str_repeat(')', 48) . '$/s';
preg_match($pattern, $subject, $matches);
?>
--EXPECTF--
Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
