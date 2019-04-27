--TEST--
Bug #53512 (NumberFormatter::setSymbol crash on bogus $attr values)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) die('skip');
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php

$badvals = array(4294901761, 2147483648, -2147483648, -1);

foreach ($badvals as $val) {
	$x = numfmt_create("en", NumberFormatter::PATTERN_DECIMAL);
	var_dump(numfmt_set_symbol($x, $val, ""));
	var_dump(intl_get_error_message());
}

?>
--EXPECT--
bool(false)
string(65) "numfmt_set_symbol: invalid symbol value: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(65) "numfmt_set_symbol: invalid symbol value: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(65) "numfmt_set_symbol: invalid symbol value: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(65) "numfmt_set_symbol: invalid symbol value: U_ILLEGAL_ARGUMENT_ERROR"
