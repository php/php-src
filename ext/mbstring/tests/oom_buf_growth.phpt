--TEST--
mbstring stores conversion-buffer pointers when growth fails
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
// A single batch expands from 128 to 384 bytes, requiring two buffer growths.
$input = str_repeat("\x80", 128);
// The tracked allocator leaves enough space for the first growth, but not the second.
$padding = str_repeat('a', 8 * 1024 * 1024 - memory_get_usage() - 384);
mb_substr_count($input, 'Z', 'CP1252');
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
