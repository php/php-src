--TEST--
cp932 cmd test
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(932, "ansi");

?>
--CONFLICTS--
file_cp932
--INI--
internal_encoding=cp932
--FILE--
<?php
/*
#vim: set fileencoding=cp932
#vim: set encoding=cp932
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";


$item = "テストマルチバイト・パス77"; // cp932 string
$prefix = create_data("file_cp932", $item, 932);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b " . $fn);

remove_data("file_cp932");

?>
--EXPECTF--
string(%d) "%s\テストマルチバイト・パス77"
bool(true)
bool(true)
テストマルチバイト・パス77
