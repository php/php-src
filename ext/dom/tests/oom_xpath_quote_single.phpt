--TEST--
DOMXPath::quote() with one quote kind doesn't reach hard memory limit
--EXTENSIONS--
dom
--INI--
memory_limit=8M
memory_hard_limit_percent=125
--FILE--
<?php
DOMXPath::quote(str_repeat("a", 4 * 1024 * 1024));

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
