--TEST--
Test grapheme_strpos-alike functions with empty needle
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

var_dump(grapheme_strpos("abc", ""));
var_dump(grapheme_stripos("abc", ""));
var_dump(grapheme_strrpos("abc", ""));
var_dump(grapheme_strripos("abc", ""));
var_dump(grapheme_strstr("abc", ""));
var_dump(grapheme_stristr("abc", ""));

?>
--EXPECT--
int(0)
int(0)
int(3)
int(3)
string(3) "abc"
bool(false)
