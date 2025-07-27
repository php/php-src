--TEST--
MessageFormatter::parseMessage() with invalid locale
--EXTENSIONS--
intl
--CREDITS--
girgias@php.net
--FILE--
<?php

$locale = 'root';
$fmt = '{0,whatever} would not work!';
$str = '4,560 monkeys on 123 trees make 37.073 monkeys per tree';

$m = MessageFormatter::parseMessage($locale, $fmt, $str);
var_dump($m);
var_dump(intl_get_error_message());

$m = msgfmt_parse_message($locale, $fmt, $str);
var_dump($m);
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(59) "Creating message formatter failed: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(59) "Creating message formatter failed: U_ILLEGAL_ARGUMENT_ERROR"
