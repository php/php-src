--TEST--
intl_error_name(): a code outside the int range is not truncated
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(intl_error_name(2**32));
var_dump(intl_error_name(2**32 + 1));
var_dump(intl_error_name(-(2**32) + 1));
var_dump(intl_error_name(PHP_INT_MIN));
var_dump(intl_error_name(2**31));
var_dump(intl_error_name(0));
?>
--EXPECT--
string(18) "[BOGUS UErrorCode]"
string(18) "[BOGUS UErrorCode]"
string(18) "[BOGUS UErrorCode]"
string(18) "[BOGUS UErrorCode]"
string(18) "[BOGUS UErrorCode]"
string(12) "U_ZERO_ERROR"
