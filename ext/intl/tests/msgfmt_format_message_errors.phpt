--TEST--
MessageFormatter::parseMessage() with invalid locale
--EXTENSIONS--
intl
--FILE--
<?php

$locale = 'root';
$fmt = '{0,whatever} would not work!';
$m = 4560;
$t = 123;
$v = [$m, $t, $m/$t];

$m = MessageFormatter::formatMessage($locale, $fmt, $v);
var_dump($m);
var_dump(intl_get_error_message());

$m = msgfmt_format_message($locale, $fmt, $v);
var_dump($m);
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(82) "MessageFormatter::formatMessage(): Creating message formatter failed: U_ZERO_ERROR"
bool(false)
string(72) "msgfmt_format_message(): Creating message formatter failed: U_ZERO_ERROR"
