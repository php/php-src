--TEST--
Bug #75574 putenv does not work properly if parameter contains non-ASCII unicode character, cp936 Windows
--SKIPIF--
<?php

if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
if (!sapi_windows_cp_set(936)) {
    die("skip Required CP 936 or compatible");
}

?>
--INI--
internal_encoding=cp936
--FILE--
<?php
/*
#vim: set fileencoding=cp936
#vim: set encoding=cp936
*/

var_dump(putenv('FOO=가'), getenv("FOO"));
var_dump(putenv('FOO=가가'), getenv("FOO"));
var_dump(putenv('FOO=가가가'), getenv("FOO"));
var_dump(putenv('FOO=가가가가'), getenv("FOO"));

var_dump(putenv('FOO=가a'), getenv("FOO"));
var_dump(putenv('FOO=가a가'), getenv("FOO"));
var_dump(putenv('FOO=가a가a'), getenv("FOO"));
var_dump(putenv('FOO=가a가a가'), getenv("FOO"));
var_dump(putenv('FOO=가a가가'), getenv("FOO"));
var_dump(putenv('FOO=가a가가가'), getenv("FOO"));
var_dump(putenv('FOO=가a가가가가'), getenv("FOO"));

?>
--EXPECT--
bool(true)
string(2) "가"
bool(true)
string(4) "가가"
bool(true)
string(6) "가가가"
bool(true)
string(8) "가가가가"
bool(true)
string(3) "가a"
bool(true)
string(5) "가a가"
bool(true)
string(6) "가a가a"
bool(true)
string(8) "가a가a가"
bool(true)
string(7) "가a가가"
bool(true)
string(9) "가a가가가"
bool(true)
string(11) "가a가가가가"
