--TEST--
Test phpinfo() displays xsl info
--SKIPIF--
<?php 
	if (!extension_loaded("xsl")) {
		die("SKIP extension gettext not loaded\n");
	}
?>
--FILE--
<?php
phpinfo();
?>
--EXPECTF--
%a
libxslt compiled against libxml Version%a
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
