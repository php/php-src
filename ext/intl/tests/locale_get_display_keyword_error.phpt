--TEST--
locale_get_display_keyword_value() error path
--EXTENSIONS--
intl
--FILE--
<?php

$locale = str_repeat('*', 256);

var_dump(Locale::getDisplayKeywordValue($locale, 'calendar', 'en'));
var_dump(intl_get_error_message());
var_dump(locale_get_display_keyword_value($locale, 'calendar', 'en'));
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(73) "Locale::getDisplayKeywordValue(): name too long: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(75) "locale_get_display_keyword_value(): name too long: U_ILLEGAL_ARGUMENT_ERROR"
