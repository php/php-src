--TEST--
Bug #53735 NumberFormatter returns NaN when converting float point
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
--FILE--
<?php

$fmt = numfmt_create("da_DK", \NumberFormatter::CURRENCY);

var_dump(numfmt_format($fmt, 5.5));
setlocale(LC_ALL, "da_DK.UTF-8");

var_dump(numfmt_format($fmt, 5.5));
var_dump(numfmt_format($fmt, "5,5"));


$fmt = new \NumberFormatter("de_DE", \NumberFormatter::DECIMAL);
var_dump($fmt->format(23.25));

$f = new NumberFormatter('hu_HU', NumberFormatter::PERCENT, '#,##0%');
var_dump($f->format(0.26));

?>
==DONE==
--EXPECTF--
string(%d) "5,50 kr%A"
string(%d) "5,50 kr%A"
string(%d) "5,00 kr%A"
string(5) "23,25"
string(3) "26%"
==DONE==
