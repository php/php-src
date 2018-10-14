--TEST--
test if bind_textdomain_codeset() returns correct value
--SKIPIF--
<?php
	if (!extension_loaded("gettext")) {
		die("skip");
	}
?>
--FILE--
<?php
	var_dump(bind_textdomain_codeset(false,false));
	var_dump(bind_textdomain_codeset('messages', "UTF-8"));

	echo "Done\n";
?>
--EXPECTF--
bool(false)
string(5) "UTF-8"
Done
--CREDITS--
Florian Holzhauer fh-pt@fholzhauer.de
PHP Testfest Berlin 2009-05-09
