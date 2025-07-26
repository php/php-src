--TEST--
Bug #72533 (locale_accept_from_http out-of-bounds access)
--EXTENSIONS--
intl
--FILE--
<?php

$http = str_repeat('x', 256);

var_dump(Locale::acceptFromHttp($http));
var_dump(intl_get_error_message());

var_dump(locale_accept_from_http($http));
var_dump(intl_get_error_message());


$http = str_repeat('en', 256);

var_dump(Locale::acceptFromHttp($http));
var_dump(intl_get_error_message());

var_dump(locale_accept_from_http($http));
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(73) "locale_accept_from_http: locale string too long: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(73) "locale_accept_from_http: locale string too long: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(73) "locale_accept_from_http: locale string too long: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(73) "locale_accept_from_http: locale string too long: U_ILLEGAL_ARGUMENT_ERROR"
