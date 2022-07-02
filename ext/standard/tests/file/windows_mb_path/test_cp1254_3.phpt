--TEST--
cp1254 cmd test
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1254, "ansi");

?>
--CONFLICTS--
file_cp1254
--INI--
default_charset=cp1254
--FILE--
<?php
/*
#vim: set fileencoding=cp1254
#vim: set encoding=cp1254
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";


$item = "�okbaytl� i�leri";
$prefix = create_data("file_cp1254", $item, 1254);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b \"" . $fn . "\"");

remove_data("file_cp1254");

?>
--EXPECTF--
string(%d) "%s\�okbaytl� i�leri"
bool(true)
bool(true)
�okbaytl� i�leri
