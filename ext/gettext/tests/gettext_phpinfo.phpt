--TEST--
Test phpinfo() displays gettext support
--SKIPIF--
<?php
	if (!extension_loaded("gettext")) {
		die("SKIP extension gettext not loaded\n");
	}
?>
--FILE--
<?php
phpinfo();
?>
--EXPECTF--
%a
%rGetText Support.*enabled%r
%a
--CREDITS--
Tim Eggert, tim@elbart.com
PHP Testfest Berlin 2009-05-09
