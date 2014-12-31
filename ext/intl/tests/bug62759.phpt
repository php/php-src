--TEST--
Bug #62759: Buggy grapheme_substr() on edge case
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
var_dump(substr('deja', 1, -4));
var_dump(substr('deja', -1, 0));
var_dump(grapheme_substr('deja', 1, -4));
var_dump(intl_get_error_message());
var_dump(grapheme_substr('deja', -1, 0));
var_dump(grapheme_substr('déjà', 1, -4));
var_dump(intl_get_error_message());
var_dump(grapheme_substr('déjà', -1, 0));
?>
--EXPECT--
bool(false)
string(0) ""
bool(false)
string(61) "grapheme_substr: invalid parameters: U_ILLEGAL_ARGUMENT_ERROR"
string(0) ""
bool(false)
string(65) "grapheme_substr: length is beyond start: U_ILLEGAL_ARGUMENT_ERROR"
string(0) ""
