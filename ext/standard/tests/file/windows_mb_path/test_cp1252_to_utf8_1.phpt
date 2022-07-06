--TEST--
Test mkdir/rmdir cp1252 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1252
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('cp1252', 'utf-8', "tsch��"); // cp1252 string
$prefix = create_data("dir_cp1252", "${item}3");
$path = $prefix . DIRECTORY_SEPARATOR . "${item}3";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp1252");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\tschüß3\tschüß4
string(9) "tschüß4"
bool(true)
string(%d) "%s\tschüß3\tschüß4"
Active code page: %d
bool(true)
