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

var_dump(putenv('FOO=��'), getenv("FOO"));
var_dump(putenv('FOO=����'), getenv("FOO"));
var_dump(putenv('FOO=������'), getenv("FOO"));
var_dump(putenv('FOO=��������'), getenv("FOO"));

var_dump(putenv('FOO=��a'), getenv("FOO"));
var_dump(putenv('FOO=��a��'), getenv("FOO"));
var_dump(putenv('FOO=��a��a'), getenv("FOO"));
var_dump(putenv('FOO=��a��a��'), getenv("FOO"));
var_dump(putenv('FOO=��a����'), getenv("FOO"));
var_dump(putenv('FOO=��a������'), getenv("FOO"));
var_dump(putenv('FOO=��a��������'), getenv("FOO"));

?>
--EXPECT--
bool(true)
string(2) "��"
bool(true)
string(4) "����"
bool(true)
string(6) "������"
bool(true)
string(8) "��������"
bool(true)
string(3) "��a"
bool(true)
string(5) "��a��"
bool(true)
string(6) "��a��a"
bool(true)
string(8) "��a��a��"
bool(true)
string(7) "��a����"
bool(true)
string(9) "��a������"
bool(true)
string(11) "��a��������"
