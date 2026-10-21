--TEST--
mb_decode_mimeheader() doesn't reach hard limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
$encoded = str_repeat("=?UTF-8?B?w6nDqcOp?= ", (int) (5500 * 1024 / 21));
mb_decode_mimeheader($encoded);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
