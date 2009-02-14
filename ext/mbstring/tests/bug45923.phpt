--TEST--
Bug #45923 (mb_st[r]ripos() offset not handled correctly)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.internal_encoding=UTF-8
--FILE--
<?php
var_dump(strpos("abc abc abc", "abc", 0));
var_dump(strpos("abc abc abc", "abc", 3));
var_dump(strpos("abc abc abc", "abc", 6));
var_dump(strpos("abc abc abc", "abc", 9));
var_dump(strpos("abc abc abc", "abc", 11));
var_dump(strpos("abc abc abc", "abc", 12));
var_dump(strpos("abc abc abc", "abc", -1));
var_dump(strpos("abc abc abc", "abc", -3));
var_dump(strpos("abc abc abc", "abc", -6));

var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", 0));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", 3));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", 6));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", 9));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", 11));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", 12));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", -1));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", -3));
var_dump(mb_strpos("●○◆ ●○◆ ●○◆", "●○◆", -6));

var_dump(stripos("abc abc abc", "abc", 0));
var_dump(stripos("abc abc abc", "abc", 3));
var_dump(stripos("abc abc abc", "abc", 6));
var_dump(stripos("abc abc abc", "abc", 9));
var_dump(stripos("abc abc abc", "abc", 11));
var_dump(stripos("abc abc abc", "abc", 12));
var_dump(stripos("abc abc abc", "abc", -1));
var_dump(stripos("abc abc abc", "abc", -3));
var_dump(stripos("abc abc abc", "abc", -6));

var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", 0));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", 3));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", 6));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", 9));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", 11));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", 12));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", -1));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", -3));
var_dump(mb_stripos("●○◆ ●○◆ ●○◆", "●○◆", -6));

var_dump(strrpos("abc abc abc", "abc", 0));
var_dump(strrpos("abc abc abc", "abc", 3));
var_dump(strrpos("abc abc abc", "abc", 6));
var_dump(strrpos("abc abc abc", "abc", 9));
var_dump(strrpos("abc abc abc", "abc", 11));
var_dump(strrpos("abc abc abc", "abc", 12));
var_dump(strrpos("abc abc abc", "abc", -1));
var_dump(strrpos("abc abc abc", "abc", -3));
var_dump(strrpos("abc abc abc", "abc", -6));

var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", 0));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", 3));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", 6));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", 9));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", 11));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", 12));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", -1));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", -3));
var_dump(mb_strrpos("●○◆ ●○◆ ●○◆", "●○◆", -6));

var_dump(strripos("abc abc abc", "abc", 0));
var_dump(strripos("abc abc abc", "abc", 3));
var_dump(strripos("abc abc abc", "abc", 6));
var_dump(strripos("abc abc abc", "abc", 9));
var_dump(strripos("abc abc abc", "abc", 11));
var_dump(strripos("abc abc abc", "abc", 12));
var_dump(strripos("abc abc abc", "abc", -1));
var_dump(strripos("abc abc abc", "abc", -3));
var_dump(strripos("abc abc abc", "abc", -6));

var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", 0));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", 3));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", 6));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", 9));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", 11));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", 12));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", -1));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", -3));
var_dump(mb_strripos("●○◆ ●○◆ ●○◆", "●○◆", -6));

?>
--EXPECTF--
int(0)
int(4)
int(8)
bool(false)
bool(false)

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)
int(0)
int(4)
int(8)
bool(false)
bool(false)

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)
int(0)
int(4)
int(8)
bool(false)
bool(false)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
int(0)
int(4)
int(8)
bool(false)
bool(false)

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)
int(8)
int(8)
int(8)
bool(false)
bool(false)

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
int(8)
int(8)
int(4)
int(8)
int(8)
int(8)
bool(false)
bool(false)

Warning: mb_strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
int(8)
int(8)
int(4)
int(8)
int(8)
int(8)
bool(false)
bool(false)

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
int(8)
int(8)
int(4)
int(8)
int(8)
int(8)
bool(false)
bool(false)

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
int(8)
int(8)
int(4)
