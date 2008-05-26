--TEST--
Bug #12647 (Locale settings affecting float parsing)
--SKIPIF--
<?php  # try to activate a german locale
if (setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1") === FALSE) {
	print "skip";
}
?>
--INI--
precision=14
--FILE--
<?php 
# activate the german locale
setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1");

echo (float)"3.14", "\n";
?>
--EXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s034.php on line %d
3,14
