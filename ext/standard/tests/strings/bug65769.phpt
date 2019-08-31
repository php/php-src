--TEST--
Bug #65769 localeconv() broken in TS builds
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip Windows only');
}
if (PHP_WINDOWS_VERSION_MAJOR < 10) {
	die("skip for Windows 10 and above");
}
?>
--FILE--
<?php

$locales = array('sve', 'french', 'us', 'ru', 'czech', 'serbian');

foreach ($locales as $locale) {
	$locale = setlocale(LC_ALL, $locale);
	$lconv = localeconv();
	var_dump(
		$locale,
		$lconv['decimal_point'],
		$lconv['thousands_sep'],
		$lconv['int_curr_symbol'],
		$lconv['currency_symbol'],
		$lconv['mon_decimal_point'],
		$lconv['mon_thousands_sep']
	);
	if ($locale === 'Swedish_Sweden.1252') {
		var_dump(in_array($lconv['mon_thousands_sep'], ['.', ' ']));
	}
	echo '++++++++++++++++++++++', "\n";
}

?>
+++DONE+++
--EXPECTF--
string(19) "Swedish_Sweden.1252"
string(1) ","
string(1) " "
string(3) "SEK"
string(2) "kr"
string(1) ","
string(1) "%c"
bool(true)
++++++++++++++++++++++
string(18) "French_France.1252"
string(1) ","
string(1) " "
string(3) "EUR"
string(1) "€"
string(1) ","
string(1) " "
++++++++++++++++++++++
string(26) "English_United States.1252"
string(1) "."
string(1) ","
string(3) "USD"
string(1) "$"
string(1) "."
string(1) ","
++++++++++++++++++++++
string(2) "ru"
string(1) ","
string(1) " "
string(3) "RUB"
string(1) "?"
string(1) ","
string(1) " "
++++++++++++++++++++++
string(%d) "Czech_Czech%s.1250"
string(1) ","
string(1) " "
string(3) "CZK"
string(2) "Kè"
string(1) ","
string(1) " "
++++++++++++++++++++++
string(19) "Serbian_Serbia.1250"
string(1) ","
string(1) "."
string(3) "RSD"
string(3) "RSD"
string(1) ","
string(1) "."
++++++++++++++++++++++
+++DONE+++
