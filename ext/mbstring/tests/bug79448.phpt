--TEST--
Bug #79448 	0 is a valid Unicode codepoint, but mb_substitute_character(0) fails
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// 0 is a valid codepoint regardless of encoding
var_dump(mb_substitute_character(0));
?>
--EXPECT--
bool(true)
