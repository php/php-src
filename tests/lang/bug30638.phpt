--TEST--
Bug #30638 (localeconv returns wrong LC_NUMERIC settings) (ok to fail on MacOS X)
--SKIPIF--
<?php  # try to activate a german locale
if (setlocale(LC_NUMERIC, "de_DE.UTF-8", "de_DE", "de", "german", "ge", "de_DE.ISO-8859-1") === FALSE) {
	print "skip setlocale() failed";
} elseif (strtolower(php_uname('s')) == 'darwin') {
    print "skip ok to fail on MacOS X";
}
?>
--FILE--
<?php 
# activate the german locale
setlocale(LC_NUMERIC, "de_DE.UTF-8", "de_DE", "de", "german", "ge", "de_DE.ISO-8859-1");

$lc = localeconv();
printf("decimal_point: %s\n", $lc['decimal_point']);
printf("thousands_sep: %s\n", $lc['thousands_sep']);
?>
--EXPECT--
decimal_point: ,
thousands_sep: .
