--TEST--
Locale settings affecting float parsing
--SKIPIF--
<?php  # try to activate a german locale
if (setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge") === FALSE) {
	print "skip";
}
?>
--POST--
--GET--
--FILE--
<?php 
# activate the german locale
setlocale(LC_NUMERIC, "de_DE", "de", "german", "ge");

echo (float)"3.14", "\n";

?>
--EXPECT--
3,14
