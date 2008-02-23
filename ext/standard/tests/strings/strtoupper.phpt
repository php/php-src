--TEST--
Test strtoupper on non-ASCII characters
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
$chars = "äöü";
// Not sure which is most portable. BSD's answer to
// this one. A small array based on PHP_OS should
// cover a majority of systems and makes the problem
// of locales transparent for the end user.
setlocale(LC_CTYPE, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1");
echo strtoupper($chars)."\n";
?>
--EXPECT--
ÄÖÜ
--UEXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
ÄÖÜ
