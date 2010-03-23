--TEST--
Bug #12647 (Locale settings affecting float parsing)
--INI--
precision=14
--SKIPIF--
<?php  # try to activate a german locale
if (setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1") === FALSE) {
	print "skip";
}
?>
--FILE--
<?php 
# activate the german locale
setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1");

echo (float)"3.14", "\n";
?>
--EXPECT--
3,14
