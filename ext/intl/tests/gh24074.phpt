--TEST--
GH-24074 (Locale::acceptFromHttp() with a list of available locales)
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '67.1') < 0) die('skip for ICU >= 67.1'); ?>
--FILE--
<?php

$header = 'fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7';

var_dump(Locale::acceptFromHttp($header, ['en_US', 'de_DE']));
var_dump(Locale::acceptFromHttp($header, ['de', 'en']));
var_dump(Locale::acceptFromHttp($header, ['de']));
var_dump(Locale::acceptFromHttp('de-CH, en;q=0.5', ['en', 'de']));
var_dump(Locale::acceptFromHttp('en-us', ['de', 'en_US']));
var_dump(Locale::acceptFromHttp('en-us', ['de', 'en-US']));
var_dump(Locale::acceptFromHttp('ja', ['en', 'de']));
var_dump(Locale::acceptFromHttp('en-us,en;q=0.5', null));
var_dump(locale_accept_from_http($header, ['de', 'en']));

var_dump(Locale::acceptFromHttp(str_repeat('a', 200), ['en']));
var_dump(Locale::acceptFromHttp('en', ['en']));
var_dump(intl_get_error_message());

$invalid = [
    [],
    [1],
    ["e\0n"],
    [str_repeat('a', 200)],
    ['en', str_repeat('a', 20)],
];

foreach ($invalid as $locales) {
    try {
        Locale::acceptFromHttp('en', $locales);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
string(5) "en_US"
string(2) "en"
string(2) "de"
string(2) "de"
string(5) "en_US"
string(5) "en-US"
bool(false)
string(5) "en_US"
string(2) "en"
bool(false)
string(2) "en"
string(12) "U_ZERO_ERROR"
ValueError: Locale::acceptFromHttp(): Argument #2 ($availableLocales) must not be empty
TypeError: Locale::acceptFromHttp(): Argument #2 ($availableLocales) must only contain string values
ValueError: Locale::acceptFromHttp(): Argument #2 ($availableLocales) must not contain any null bytes
ValueError: Locale::acceptFromHttp(): Argument #2 ($availableLocales) must not contain locales longer than 156 characters
ValueError: Locale::acceptFromHttp(): Argument #2 ($availableLocales) must only contain valid locales
