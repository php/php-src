--TEST--
iconv_strrpos() charset parameter length checks (CVE-2007-4840)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$a = str_repeat("/", 9000000);
var_dump(iconv_strrpos("a", "b", $a));
?>
--EXPECTF--

Warning: iconv_strrpos(): Charset parameter exceeds the maximum allowed length of %d characters in %s on line %d
bool(false)
