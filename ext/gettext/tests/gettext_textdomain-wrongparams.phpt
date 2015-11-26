--TEST--
Check how textdomain() with wrong parameters behaves.
--SKIPIF--
<?php 
	if (!extension_loaded("gettext")) {
		die("skip\n");
	}
	if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
		die("skip en_US.UTF-8 locale not supported.");
	}
?>
--FILE--
<?php // $Id$

chdir(dirname(__FILE__));
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain ("messages", "./locale");
textdomain (array());

?>
--EXPECTF--
Warning: textdomain() expects parameter 1 to be string, array given in %s on line 6
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09