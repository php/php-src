--TEST--
Test preg_match() function : error conditions - Malformed UTF-8 offset
--FILE--
<?php
preg_match('/a/u', "\xE3\x82\xA2", $m, 0, 1);
var_dump(preg_last_error() === PREG_BAD_UTF8_OFFSET_ERROR);
var_dump(preg_last_error_msg() === 'The offset did not correspond to the beginning of a valid UTF-8 code point');
?>
--EXPECT--
bool(true)
bool(true)
