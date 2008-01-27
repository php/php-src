--TEST--
money_format test
--SKIPIF--
<?php
	if (!function_exists('money_format') || !function_exists('setlocale')) {
		die("SKIP money_format - not supported\n");
	}

if (setlocale(LC_MONETARY, 'en_US') == false) {
	die('skip en_US locale not available');
}
?>
--FILE--
<?php
setlocale(LC_MONETARY, 'en_US');
var_dump( money_format("X%nY", 3.1415));
var_dump(money_format("AAAAA%n%n%n%n", NULL));
?>
--EXPECTF--
string(7) "X$3.14Y"

Warning: money_format(): Only a single %ci or %cn token can be used in %s on line %d
bool(false)
--UEXPECTF--
Strict Standards: setlocale(): deprecated in Unicode mode, please use ICU locale functions in /home/jani/src/php6/ext/standard/tests/strings/moneyformat.php on line 2
unicode(7) "X$3.14Y"

Warning: money_format(): Only a single %i or %n token can be used in /home/jani/src/php6/ext/standard/tests/strings/moneyformat.php on line 4
bool(false)
