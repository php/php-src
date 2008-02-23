--TEST--
Test strtotitle
--SKIPIF--
<?php
if (!setlocale(LC_CTYPE, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1")) {
        die("skip locale needed for this test is not supported on this platform");
}
?>
--INI--
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
--FILE--
<?php
declare(encoding="latin1");
setlocale(LC_ALL, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1");

$strings = array( "ßen", "året", "året ßen" );

foreach( $strings as $string )
{
	echo ucwords( $string ), "\n";
	echo strtotitle( $string ), "\n";
}
?>
--EXPECT--
ßen
ßen
Året
Året
Året ßen
Året ßen
--UEXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
SSen
Ssen
Året
Året
Året SSen
Året Ssen
