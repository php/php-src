--TEST--
GitHub #11658 MessageFormatter::format() leaks memory
--EXTENSIONS--
intl
--FILE--
<?php

$s = MessageFormatter::formatMessage('en', 'some {wrong.format}', []);
var_dump($s);
echo intl_get_error_message(), PHP_EOL;

$s = msgfmt_format_message('en', 'some {wrong.format}', []);
var_dump($s);
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
MessageFormatter::formatMessage(): pattern syntax error (parse error at offset 6, after "some {", before or at "wrong.format}"): U_PATTERN_SYNTAX_ERROR
bool(false)
msgfmt_format_message(): pattern syntax error (parse error at offset 6, after "some {", before or at "wrong.format}"): U_PATTERN_SYNTAX_ERROR
