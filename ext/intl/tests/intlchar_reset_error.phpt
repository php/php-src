--TEST--
IntlChar methods reset intl error on success
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(IntlChar::digit('a', 1));
var_dump(intl_get_error_code() !== U_ZERO_ERROR);

var_dump(IntlChar::forDigit(1) === ord('1'));
var_dump(intl_get_error_code() === U_ZERO_ERROR);
var_dump(intl_get_error_message() === 'U_ZERO_ERROR');
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
