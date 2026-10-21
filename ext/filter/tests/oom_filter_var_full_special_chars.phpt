--TEST--
filter_var(filter: FILTER_SANITIZE_FULL_SPECIAL_CHARS) doesn't reach hard limit
--EXTENSIONS--
filter
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--FILE--
<?php
$value = str_repeat("'", 2 * 1024 * 1024);
filter_var($value, FILTER_SANITIZE_FULL_SPECIAL_CHARS);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
