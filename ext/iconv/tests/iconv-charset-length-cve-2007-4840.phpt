--TEST--
iconv() charset parameter length checks (CVE-2007-4840)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$a = str_repeat("/", 9000000);
var_dump(iconv($a, "b", "test"));
var_dump(iconv("x", $a, "test"));
?>
--EXPECTF--

Warning: iconv(): Charset parameter exceeds the maximum allowed length of %d characters in %s on line %d
bool(false)

Warning: iconv(): Charset parameter exceeds the maximum allowed length of %d characters in %s on line %d
bool(false)
