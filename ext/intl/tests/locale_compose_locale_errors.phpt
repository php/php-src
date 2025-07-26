--TEST--
locale_compose_locale() errors
--EXTENSIONS--
intl
--FILE--
<?php

$parts1 = [
    Locale::LANG_TAG => 45,
    Locale::REGION_TAG => false,
    Locale::SCRIPT_TAG => 15,
];

var_dump(Locale::composeLocale($parts1));
var_dump(intl_get_error_message());
var_dump(locale_compose($parts1));
var_dump(intl_get_error_message());

$parts2 = [
    Locale::LANG_TAG => 'de',
    Locale::REGION_TAG => 'DE',
    'private0' => 13,
    'variant1' => array(),
    'extlang2' => false
];

var_dump(Locale::composeLocale($parts2));
var_dump(intl_get_error_message());
var_dump(locale_compose($parts2));
var_dump(intl_get_error_message());

$parts3 = [
    Locale::REGION_TAG => 'DE',
];

try {
    var_dump(Locale::composeLocale($parts3));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(locale_compose($parts3));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(false)
string(81) "locale_compose: parameter array element is not a string: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(81) "locale_compose: parameter array element is not a string: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(81) "locale_compose: parameter array element is not a string: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(81) "locale_compose: parameter array element is not a string: U_ILLEGAL_ARGUMENT_ERROR"
ValueError: Locale::composeLocale(): Argument #1 ($subtags) must contain a "language" key
ValueError: locale_compose(): Argument #1 ($subtags) must contain a "language" key
