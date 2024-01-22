--TEST--
GitHub #12020 intl_get_error_message() broken after MessageFormatter::formatMessage() fails
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(\MessageFormatter::formatMessage('en', 'some message with {invalid format}', []), intl_get_error_message());
var_dump(\MessageFormatter::formatMessage('en', 'some {wrong.format}', []), intl_get_error_message());

var_dump(msgfmt_format_message('en', 'some message with {invalid format}', []), intl_get_error_message());
var_dump(msgfmt_format_message('en', 'some {wrong.format}', []), intl_get_error_message());
?>
--EXPECT--
bool(false)
string(128) "pattern syntax error (parse error at offset 19, after " message with {", before or at "invalid format}"): U_PATTERN_SYNTAX_ERROR"
bool(false)
string(116) "pattern syntax error (parse error at offset 6, after "some {", before or at "wrong.format}"): U_PATTERN_SYNTAX_ERROR"
bool(false)
string(128) "pattern syntax error (parse error at offset 19, after " message with {", before or at "invalid format}"): U_PATTERN_SYNTAX_ERROR"
bool(false)
string(116) "pattern syntax error (parse error at offset 6, after "some {", before or at "wrong.format}"): U_PATTERN_SYNTAX_ERROR"
