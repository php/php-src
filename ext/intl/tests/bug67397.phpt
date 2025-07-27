--TEST--
Bug #67397 (Buffer overflow in locale_get_display_name->uloc_getDisplayName (libicu 4.8.1))
--EXTENSIONS--
intl
--FILE--
<?php

$locale = str_repeat('*', 256);
$dispLocale= 'en_us';

var_dump(Locale::getDisplayName($locale, $dispLocale));
var_dump(intl_get_error_message());
var_dump(locale_get_display_name($locale, $dispLocale));
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(65) "locale_get_display_name : name too long: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(65) "locale_get_display_name : name too long: U_ILLEGAL_ARGUMENT_ERROR"
