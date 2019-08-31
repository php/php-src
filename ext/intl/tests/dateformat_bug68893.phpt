--TEST--
Bug #68893 Stackoverflow in datefmt_create
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

$f = datefmt_create("en_us", -10000000, 1);
var_dump($f, intl_get_error_message());

$f = datefmt_create("en_us", 1, -10000000);
var_dump($f, intl_get_error_message());

?>
--EXPECT--
NULL
string(67) "datefmt_create: invalid date format style: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(67) "datefmt_create: invalid time format style: U_ILLEGAL_ARGUMENT_ERROR"
