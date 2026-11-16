--TEST--
iconv_mime_decode() doesn't reach hard limit
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--INI--
memory_limit=8M
--FILE--
<?php
iconv_mime_decode('x: ' . str_repeat('a', 2 * 1024 * 1024), 0, 'UTF-32');
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
