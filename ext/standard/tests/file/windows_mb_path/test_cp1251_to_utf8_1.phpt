--TEST--
Test mkdir/rmdir CP1251 to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1251
#vim: set encoding=cp1251
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("dir_cp1251");
$path = $prefix . DIRECTORY_SEPARATOR . "������3"; // cp1251 string
$pathw = iconv('cp1251', 'utf-8', $path);

$subpath = $path . DIRECTORY_SEPARATOR . "������4";
$subpathw = iconv('cp1251', 'utf-8', $subpath);

/* The mb dirname exists*/
var_dump(file_exists($pathw));

var_dump(mkdir($subpathw));
var_dump(file_exists($subpathw));

get_basename_with_cp($subpathw, 65001);

var_dump(rmdir($subpathw));
remove_data("dir_cp1251");

?>
===DONE===
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
===DONE===
