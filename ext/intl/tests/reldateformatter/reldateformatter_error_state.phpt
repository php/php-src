--TEST--
IntlRelativeDateTimeFormatter synchronizes and resets ICU error state
--EXTENSIONS--
intl
--FILE--
<?php

$formatter = new IntlRelativeDateTimeFormatter('en_US');

var_dump($formatter->combineDateAndTime("\x80", '3:45 PM'));
var_dump($formatter->getErrorCode() === U_INVALID_CHAR_FOUND);
var_dump(intl_get_error_code() === $formatter->getErrorCode());
var_dump(str_contains($formatter->getErrorMessage(), 'Failed to convert relative date to UTF-16'));

var_dump($formatter->format(1, IntlRelativeDateTimeFormatter::UNIT_DAY));
var_dump($formatter->getErrorCode());
var_dump(intl_get_error_code());

ini_set('intl.use_exceptions', '1');
try {
    $formatter->combineDateAndTime('today', "\x80");
} catch (IntlException $e) {
    echo $e::class, ': ', str_contains($e->getMessage(), 'Failed to convert time to UTF-16') ? 'conversion failed' : 'unexpected', "\n";
}

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
string(8) "tomorrow"
int(0)
int(0)
IntlException: conversion failed
