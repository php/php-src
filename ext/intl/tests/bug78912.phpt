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
# Also some tests for bug #76093 while we're at it.
var_dump($nf->formatCurrency(9999999999999999999, 'USD')); # gets rounded
var_dump($nf->formatCurrency('-12345.67', 'USD'));
var_dump($nf->formatCurrency('9999999999999999999', 'USD')); # not rounded!
?>
--EXPECT--
string(12) "($12,345.67)"
string(30) "$10,000,000,000,000,000,000.00"
string(12) "($12,345.67)"
string(29) "$9,999,999,999,999,999,999.00"
