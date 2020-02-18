--TEST--
Test preg_match() function : error conditions - Malformed UTF-8 offset
--FILE--
<?php
preg_match('/a/u', "\xE3\x82\xA2", $m, 0, 1);
var_dump(preg_last_error() === PREG_BAD_UTF8_OFFSET_ERROR);
var_dump(preg_last_error_msg() === 'Malformed UTF-8 offset, possibly incorrectly encoded');
?>
--EXPECT--
bool(true)
bool(true)
