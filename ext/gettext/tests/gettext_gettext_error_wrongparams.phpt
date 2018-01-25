--TEST--
Check how gettext() with wrong parameters behaves.
--SKIPIF--
<?php
	if (!extension_loaded("gettext")) {
		die("skip extension gettext not loaded\n");
	}
?>
--FILE--
<?php
gettext (array());
?>
--EXPECTF--
Warning: gettext() expects parameter 1 to be string, array given in %s on line 2
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-09
