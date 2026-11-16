--TEST--
UConverter::transcode() doesn't reach hard limit
--EXTENSIONS--
intl
--INI--
memory_limit=8M
--FILE--
<?php
$source = str_repeat("A", 2 * 1024 * 1024);
UConverter::transcode($source, "UTF-32BE", "ISO-8859-1");
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
