--TEST--
iconv_mime_encode() line-length doesn't reach hard limit
--EXTENSIONS--
iconv
--INI--
memory_limit=8M
--FILE--
<?php
iconv_mime_encode('x', 'y', ['line-length' => PHP_INT_MAX]);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
