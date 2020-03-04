--TEST--
Test mkdir/rmdir big5 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(950, "ansi");

?>
--CONFLICTS--
dir_big5
--INI--
internal_encoding=big5
--FILE--
<?php
/*
#vim: set fileencoding=big5
#vim: set encoding=big5
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "測試多字節路徑"; // BIG5 string
$prefix = create_data("dir_big5", $item . "5", 950);
$path = $prefix . DIRECTORY_SEPARATOR . "${item}5";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 950);

var_dump(rmdir($subpath));
remove_data("dir_big5");

?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 950
getting basename of %s測試多字節路徑5\測試多字節路徑4
string(%d) "測試多字節路徑4"
bool(true)
string(%d) "%s測試多字節路徑5\測試多字節路徑4"
Active code page: %d
bool(true)
===DONE===
