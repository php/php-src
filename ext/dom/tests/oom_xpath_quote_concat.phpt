--TEST--
DOMXPath::quote() with both quote kinds doesn't reach hard memory limit
--EXTENSIONS--
dom
--INI--
memory_limit=8M
--FILE--
<?php
/* A string holding both quote kinds takes the concat() path */
DOMXPath::quote(str_repeat("'\"", 2 * 1024 * 1024));

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
