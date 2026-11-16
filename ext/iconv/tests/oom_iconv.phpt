--TEST--
iconv() doesn't reach hard limit
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--INI--
memory_limit=16M
--FILE--
<?php
iconv('ASCII', 'UTF-32', str_repeat('a', 4 * 1024 * 1024));
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
