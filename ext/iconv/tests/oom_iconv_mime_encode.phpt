--TEST--
iconv_mime_encode() doesn't reach hard limit
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
$value = str_repeat("\xFF", 2 * 1024 * 1024); // 2 MB == memory_limit / 4
iconv_mime_encode("A", $value, [
    "scheme" => "Q",
    "input-charset" => "ISO-8859-1",
    "output-charset" => "ISO-8859-1",
]);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
