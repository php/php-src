--TEST--
iconv_strlen() charset parameter length checks (CVE-2007-4840)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$a = str_repeat("/", 9791999);
var_dump(iconv_strlen(1, $a));
?>
--EXPECTF--

Warning: iconv_strlen(): Charset parameter exceeds the maximum allowed length of %d characters in %s on line %d
bool(false)
