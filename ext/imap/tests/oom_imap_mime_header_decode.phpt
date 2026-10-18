--TEST--
imap_mime_header_decode() doesn't reach hard limit
--EXTENSIONS--
imap
--INI--
memory_limit=8M
--FILE--
<?php
$header = str_repeat("=?a?q?b?=", 233016);
imap_mime_header_decode($header);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
