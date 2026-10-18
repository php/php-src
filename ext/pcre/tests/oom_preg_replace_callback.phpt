--TEST--
preg_replace_callback() doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php
$subject = str_repeat("a", 5 * 1024 * 1024);
preg_replace_callback('/^/', function () use ($subject) { return $subject; }, $subject);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
