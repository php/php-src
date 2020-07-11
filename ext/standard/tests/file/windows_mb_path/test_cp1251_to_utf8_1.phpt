--TEST--
Test mkdir/rmdir CP1251 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1251
--FILE--
<?php
/*
#vim: set fileencoding=cp1251
#vim: set encoding=cp1251
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('cp1251', 'utf-8', "������"); // cp1251 string
$prefix = create_data("dir_cp1251", $item . "3");
$path = $prefix . DIRECTORY_SEPARATOR . $item . "3";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp1251");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\привет3\привет4
string(13) "привет4"
bool(true)
string(%d) "%s\привет3\привет4"
Active code page: %d
bool(true)
