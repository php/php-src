--TEST--
Check if textdomain() returns the new domain
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
<?php

chdir(dirname(__FILE__));
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain ("messages", "./locale");
echo textdomain('test'), "\n";
echo textdomain(null), "\n";
echo textdomain('foo'), "\n";
?>
--EXPECT--

test
test
foo
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09