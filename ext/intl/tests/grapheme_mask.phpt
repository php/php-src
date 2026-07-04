--TEST--
grapheme_mask() - Basic functionality
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled');
?>
--FILE--
<?php
var_dump(grapheme_mask("Hello World", "X"));
var_dump(grapheme_mask("Hello World", "X", 3));
var_dump(grapheme_mask("Hello World", "X", 3, 2));
var_dump(grapheme_mask("Hello World", "X", -5, 3));
var_dump(grapheme_mask("Hello World", "X", 0, 0));
var_dump(grapheme_mask("", "X"));
var_dump(grapheme_mask("Hello World", "👍"));
var_dump(grapheme_mask("Hello World", "ab")); // should throw ValueError
?>
--EXPECTF--
string(11) "XXXXX XXXXX"
string(11) "HelXX XXXXX"
string(11) "HelXXo XXXXX"
string(11) "Hello WXXld"
string(11) "Hello World"
string(0) ""
string(11) "👍👍👍👍👍 👍👍👍👍👍"

Warning: grapheme_mask(): Argument #2 ($mask_char) must be exactly one grapheme cluster in %s on line %d
bool(false)
