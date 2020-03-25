--TEST--
Test mkdir/rmdir big5 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_big5
--FILE--
<?php
/*
#vim: set fileencoding=big5
#vim: set encoding=big5
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('big5', 'utf-8', "���զh�r�`���|"); // BIG5 string
$prefix = create_data("dir_big5", $item . "5");
$path = $prefix . DIRECTORY_SEPARATOR . "${item}5";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_big5");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\測試多字節路徑5\測試多字節路徑4
string(22) "測試多字節路徑4"
bool(true)
string(%d) "%s\測試多字節路徑5\測試多字節路徑4"
Active code page: %d
bool(true)
