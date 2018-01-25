--TEST--
Check how dgettext() with wrong parameter types and wrong parameter cou types and wrong parameter count behaves.
--SKIPIF--
<?php
	if (!extension_loaded("gettext")) {
		die("skip extension gettext not loaded\n");
	}
    if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
        die("skip en_US.UTF-8 locale not supported.");
    }

?>
--FILE--
<?php
    chdir(dirname(__FILE__));
    setlocale(LC_ALL, 'en_US.UTF-8');
    dgettext ('foo');
    dgettext ();

    dgettext(array(), 'foo');
    dgettext('foo', array());

?>
--EXPECTF--
Warning: dgettext() expects exactly 2 parameters, 1 given in %s.php on line %d

Warning: dgettext() expects exactly 2 parameters, 0 given in %s.php on line %d

Warning: dgettext() expects parameter 1 to be string, array given in %s.php on line %d

Warning: dgettext() expects parameter 2 to be string, array given in %s.php on line %d
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-09
