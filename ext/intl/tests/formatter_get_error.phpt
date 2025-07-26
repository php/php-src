--TEST--
numfmt_get_error_message/code()
--EXTENSIONS--
intl
--FILE--
<?php

$nf = new NumberFormatter("en_US", NumberFormatter::CURRENCY);
$nf->parseCurrency('123.45', $currency);
var_dump($currency);
var_dump($nf->getErrorMessage());
var_dump($nf->getErrorCode());

$pos = 0;
$nf->parseCurrency('123.45', $currency, $pos);
var_dump($currency);
var_dump($nf->getErrorMessage());
var_dump($nf->getErrorCode());

$nf = numfmt_create("en_US", NumberFormatter::CURRENCY);
var_dump(numfmt_parse_currency($nf, '123.45', $currency));
var_dump($currency);
var_dump($nf->getErrorMessage());
var_dump($nf->getErrorCode());

$pos = 0;
var_dump(numfmt_parse_currency($nf, '123.45', $currency, $pos));
var_dump($currency);
var_dump($nf->getErrorMessage());
var_dump($nf->getErrorCode());

?>
--EXPECT--
NULL
string(36) "Number parsing failed: U_PARSE_ERROR"
int(9)
NULL
string(36) "Number parsing failed: U_PARSE_ERROR"
int(9)
bool(false)
NULL
string(36) "Number parsing failed: U_PARSE_ERROR"
int(9)
bool(false)
NULL
string(36) "Number parsing failed: U_PARSE_ERROR"
int(9)
