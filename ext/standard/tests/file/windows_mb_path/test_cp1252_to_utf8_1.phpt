--TEST--
Test mkdir/rmdir cp1252 to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("dir_cp1252");
$path = $prefix . DIRECTORY_SEPARATOR . "tsch��3"; // cp1252 string
$pathw = iconv('cp1252', 'utf-8', $path);

$subpath = $path . DIRECTORY_SEPARATOR . "tsch��4";
$subpathw = iconv('cp1252', 'utf-8', $subpath);

/* The mb dirname exists*/
var_dump(file_exists($pathw));

var_dump(mkdir($subpathw));
var_dump(file_exists($subpathw));

get_basename_with_cp($subpathw, 65001);

var_dump(rmdir($subpathw));
remove_data("dir_cp1252");

?>
===DONE===
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
===DONE===
