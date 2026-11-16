--TEST--
UConverter::convert() doesn't reach hard limit
--EXTENSIONS--
intl
--INI--
memory_limit=8M
--FILE--
<?php
$converter = new UConverter("UTF-32BE", "ISO-8859-1");
$source = str_repeat("A", 2 * 1024 * 1024);
$converter->convert($source);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
