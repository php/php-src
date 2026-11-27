--TEST--
IntlDateFormatter parse functions reject an offset below INT_MIN
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (PHP_INT_SIZE <= 4) die('skip requires a 64-bit zend_long');
?>
--FILE--
<?php
/* -4294967295 used to be truncated to an int32_t of 1, so parsing started
 * at offset 1 instead of failing. */
$fmt = new IntlDateFormatter('en_US', IntlDateFormatter::FULL, IntlDateFormatter::FULL, 'UTC', null, 'yyyy');

$offset = -4294967295;
var_dump(datefmt_parse($fmt, '12026', $offset), $offset, intl_get_error_message());

$offset = -4294967295;
var_dump($fmt->parseToCalendar('12026', $offset), $offset, intl_get_error_message());

$offset = -4294967295;
var_dump(datefmt_localtime($fmt, '12026', $offset), $offset, intl_get_error_message());
?>
--EXPECTF--
bool(false)
int(-4294967295)
string(%d) "%SString index is out of valid range.: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
int(-4294967295)
string(%d) "%SString index is out of valid range.: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
int(-4294967295)
string(%d) "%SString index is out of valid range.: U_ILLEGAL_ARGUMENT_ERROR"
