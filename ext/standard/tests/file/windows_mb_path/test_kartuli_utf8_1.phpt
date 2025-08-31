--TEST--
Test mkdir/rmdir Kartuli UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_kartuli
--FILE--
<?php
/*
#vim: set fileencoding=utf-8
#vim: set encoding=utf-8
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "ქართველები";
$prefix = create_data("dir_kartuli", "{$item}42");
$path = $prefix . DIRECTORY_SEPARATOR . "{$item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "{$item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_kartuli");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\ქართველები42\ქართველები4
string(31) "ქართველები4"
bool(true)
string(%d) "%s\ქართველები42\ქართველები4"
Active code page: %d
bool(true)
