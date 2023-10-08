--TEST--
Zero byte test
--FILE--
<?php

var_dump(preg_match("\0//i", ""));
var_dump(preg_match("/\0/i", ""));
var_dump(preg_match("/\0/i", "\0"));
var_dump(preg_match("//\0i", ""));
var_dump(preg_match("//i\0", ""));
var_dump(preg_match("/\\\0/i", ""));
var_dump(preg_match("/\\\0/i", "\\\0"));

var_dump(preg_match("\0[]i", ""));
var_dump(preg_match("[\0]i", ""));
var_dump(preg_match("[\0]i", "\0"));
var_dump(preg_match("[]\0i", ""));
var_dump(preg_match("[]i\0", ""));
var_dump(preg_match("[\\\0]i", ""));
var_dump(preg_match("[\\\0]i", "\\\0"));

var_dump(preg_match("/abc\0def/", "abc"));
var_dump(preg_match("/abc\0def/", "abc\0def"));
var_dump(preg_match("/abc\0def/", "abc\0fed"));

var_dump(preg_match("[abc\0def]", "abc"));
var_dump(preg_match("[abc\0def]", "abc\0def"));
var_dump(preg_match("[abc\0def]", "abc\0fed"));

?>
--EXPECTF--
Warning: preg_match(): Delimiter must not be alphanumeric, backslash, or NUL in %snull_bytes.php on line 3
bool(false)
int(0)
int(1)

Warning: preg_match(): NUL is not a valid modifier in %snull_bytes.php on line 6
bool(false)

Warning: preg_match(): NUL is not a valid modifier in %snull_bytes.php on line 7
bool(false)
int(0)
int(1)

Warning: preg_match(): Delimiter must not be alphanumeric, backslash, or NUL in %snull_bytes.php on line 11
bool(false)
int(0)
int(1)

Warning: preg_match(): NUL is not a valid modifier in %snull_bytes.php on line 14
bool(false)

Warning: preg_match(): NUL is not a valid modifier in %snull_bytes.php on line 15
bool(false)
int(0)
int(1)
int(0)
int(1)
int(0)
int(0)
int(1)
int(0)
