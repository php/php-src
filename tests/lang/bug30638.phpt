--TEST--
Bug #30638 (localeconv returns wrong LC_NUMERIC settings) (ok to fail on MacOS X)
--SKIPIF--
<?php  # try to activate a german locale
if (setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge") === FALSE) {
	print "skip";
}
?>
--FILE--
<?php 
# activate the german locale
setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge");

$lc = localeconv();
printf("decimal_point: %s\n", $lc['decimal_point']);
printf("thousands_sep: %s\n", $lc['thousands_sep']);
?>
--EXPECT--
decimal_point: ,
thousands_sep: .
--UEXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d

Deprecated: localeconv(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
decimal_point: ,
thousands_sep: .
