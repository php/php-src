--TEST--
Test strtoupper on non-ASCII characters
--POST--
--GET--
--FILE--
<?php
$chars = "ЮКО";
setlocale(LC_ALL, "C");
// Not sure which is most portable. BSD's answer to
// this one. A small array based on PHP_OS should
// cover a majority of systems and makes the problem
// of locales transparent for the end user.
setlocale(LC_CTYPE, "ISO8859-1");
print(strtoupper($chars));
?>
--EXPECT--
дко
