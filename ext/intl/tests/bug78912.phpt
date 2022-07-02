--TEST--
Request #78912 (INTL Support for accounting format)
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '53.0') < 0) die('skip for ICU >= 53.0');
?>
--FILE--
<?php
$nf = new NumberFormatter('en_US', NumberFormatter::CURRENCY_ACCOUNTING);
var_dump($nf->formatCurrency(-12345.67, 'USD'));
?>
--EXPECT--
string(12) "($12,345.67)"
