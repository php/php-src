--TEST--
test if bind_textdomain_codeset() fails on wrong param count
--SKIPIF--
<?php 
	if (!extension_loaded("gettext")) {
		die("skip"); 
	}
?>
--FILE--
<?php
	bind_textdomain_codeset('messages');
	bind_textdomain_codeset('messages','foo','bar');

	echo "Done\n";
?>
--EXPECTF--	
Warning: bind_textdomain_codeset() expects exactly 2 parameters, 1 given in %s on line %d

Warning: bind_textdomain_codeset() expects exactly 2 parameters, 3 given in %s on line %d
Done
--CREDITS--
Florian Holzhauer fh-pt@fholzhauer.de
PHP Testfest Berlin 2009-05-09