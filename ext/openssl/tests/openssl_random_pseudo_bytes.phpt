--TEST--
openssl_random_pseudo_bytes() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
for ($i = 0; $i < 10; $i++) {
	var_dump(bin2hex(openssl_random_pseudo_bytes($i, $strong)));
}

?>
--EXPECTF--
unicode(0) ""
unicode(2) "%s"
unicode(4) "%s"
unicode(6) "%s"
unicode(8) "%s"
unicode(10) "%s"
unicode(12) "%s"
unicode(14) "%s"
unicode(16) "%s"
unicode(18) "%s"