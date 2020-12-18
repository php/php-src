--TEST--
Test grapheme_strpos-alike functions with empty needle
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

var_dump(grapheme_strpos("abc", ""));
var_dump(grapheme_strpos("abc", "", -1));
var_dump(grapheme_stripos("abc", ""));
var_dump(grapheme_stripos("abc", "", -1));
var_dump(grapheme_strrpos("abc", ""));
var_dump(grapheme_strrpos("abc", "", -1));
var_dump(grapheme_strripos("abc", ""));
var_dump(grapheme_strripos("abc", "", 1));
var_dump(grapheme_strstr("abc", ""));
var_dump(grapheme_strstr("abc", "", true));
var_dump(grapheme_stristr("abc", ""));
var_dump(grapheme_stristr("abc", "", true));
var_dump(grapheme_strpos("äbc", ""));
var_dump(grapheme_strpos("äbc", "", -1));
var_dump(grapheme_stripos("äbc", ""));
var_dump(grapheme_stripos("äbc", "", -1));
var_dump(grapheme_strrpos("äbc", ""));
var_dump(grapheme_strrpos("äbc", "", -1));
var_dump(grapheme_strripos("äbc", ""));
var_dump(grapheme_strripos("äbc", "", 1));
var_dump(grapheme_strstr("äbc", ""));
var_dump(grapheme_strstr("äbc", "", true));
var_dump(grapheme_stristr("äbc", ""));
var_dump(grapheme_stristr("äbc", "", true));

?>
--EXPECT--
int(0)
int(2)
int(0)
int(2)
int(3)
int(2)
int(3)
int(3)
string(3) "abc"
string(0) ""
string(3) "abc"
string(0) ""
int(0)
int(2)
int(0)
int(2)
int(3)
int(2)
int(3)
int(3)
string(4) "äbc"
string(0) ""
string(4) "äbc"
string(0) ""
