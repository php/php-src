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
string(47) "Creating message formatter failed: U_ZERO_ERROR"
bool(false)
string(47) "Creating message formatter failed: U_ZERO_ERROR"
