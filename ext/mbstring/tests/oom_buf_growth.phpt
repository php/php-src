--TEST--
mbstring stores conversion-buffer pointers when growth fails
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--SKIPIF--
<?php
// This regression test was written to cover a memory-corruption bug, but it only
// actually detects the bug when we run the test under a memory sanitizer like ASan.
if (!getenv('SKIP_ASAN')) {
    die('skip only discriminates under an ASan-instrumented build');
}
if (PHP_INT_SIZE !== 8) {
    die('skip 64-bit only');
}
?>
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
