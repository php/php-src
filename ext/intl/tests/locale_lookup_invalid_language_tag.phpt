--TEST--
Locale::lookup() returns null for invalid language tags
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(Locale::lookup([''], 'de-DE', false, 'en-US'));
var_dump(intl_get_error_message());

var_dump(locale_lookup([''], 'de-DE', false, 'en-US'));
var_dump(intl_get_error_message());

ini_set('intl.use_exceptions', '1');

try {
    Locale::lookup([''], 'de-DE', false, 'en-US');
} catch (IntlException $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    locale_lookup([''], 'de-DE', false, 'en-US');
} catch (IntlException $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
NULL
string(57) "unable to canonicalize lang_tag: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(57) "unable to canonicalize lang_tag: U_ILLEGAL_ARGUMENT_ERROR"
unable to canonicalize lang_tag
unable to canonicalize lang_tag
