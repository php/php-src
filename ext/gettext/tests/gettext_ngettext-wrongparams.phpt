--TEST--
Check how ngettext() with wrong parameters behaves.
--SKIPIF--
<?php
	if (!extension_loaded("gettext")) {
		die("SKIP extension gettext not loaded\n");
	}

?>
--FILE--
<?php
ngettext(array(), "", 1);
ngettext("", array(), 1);
ngettext("", "", array());
ngettext();
ngettext("");
ngettext("", "");
?>
--EXPECTF--
Warning: ngettext() expects parameter 1 to be string, array given in %s on line 2

Warning: ngettext() expects parameter 2 to be string, array given in %s on line 3

Warning: ngettext() expects parameter 3 to be integer, array given in %s on line 4

Warning: ngettext() expects exactly 3 parameters, 0 given in %s on line 5

Warning: ngettext() expects exactly 3 parameters, 1 given in %s on line 6

Warning: ngettext() expects exactly 3 parameters, 2 given in %s on line 7
--CREDITS--
Tim Eggert, tim@elbart.com
PHP Testfest Berlin 2009-05-09
