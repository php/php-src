--TEST--
Gettext basic test
--SKIPIF--
<?php
	error_reporting(0);
	if (!extension_loaded("gettext")) {
		die("skip\n");
	}
	if (!setlocale(LC_ALL, 'de_DE')) {
		die("skip de_DE locale not supported.");
	}
?>
--FILE--
<?php
error_reporting(~E_STRICT);

chdir(dirname(__FILE__));
putenv("LANGUAGE=de");
setlocale(LC_ALL, 'de_DE');
bindtextdomain ("messages", "./locale");
textdomain ("messages");
var_dump(gettext("Basic test"));
var_dump(_("Basic test"));
var_dump(textdomain(0));
var_dump(textdomain(""));

?>
--EXPECT--
string(14) "Einfacher Test"
string(14) "Einfacher Test"
string(8) "messages"
string(8) "messages"
--UEXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
string(14) "Einfacher Test"
string(14) "Einfacher Test"
unicode(8) "messages"
unicode(8) "messages"
