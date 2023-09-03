--TEST--
GitHub #11658 MessageFormatter::format() leaks memory
--EXTENSIONS--
intl
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

$s = MessageFormatter::formatMessage('en', 'some {wrong.format}', []);
var_dump($s);

$s = msgfmt_format_message('en', 'some {wrong.format}', []);
var_dump($s);
?>
--EXPECTF--
Warning: MessageFormatter::formatMessage(): pattern syntax error (parse error at offset 6, after "some {", before or at "wrong.format}") in %s on line %d
bool(false)

Warning: msgfmt_format_message(): pattern syntax error (parse error at offset 6, after "some {", before or at "wrong.format}") in %s on line %d
bool(false)
