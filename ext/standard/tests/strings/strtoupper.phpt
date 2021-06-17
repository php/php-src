--TEST--
Test strtoupper on non-ASCII characters
--SKIPIF--
<?php
if (!setlocale(LC_CTYPE, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1")) {
    die("skip requires locale ['de_DE', 'de', 'german', 'ge', 'de_DE.ISO8859-1', 'ISO8859-1']");
}
?>
--FILE--
<?php
$chars = "���";
// Not sure which is most portable. BSD's answer to
// this one. A small array based on PHP_OS should
// cover a majority of systems and makes the problem
// of locales transparent for the end user.
setlocale(LC_CTYPE, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1");
echo strtoupper($chars)."\n";
?>
--EXPECT--
���
