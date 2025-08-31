--TEST--
Test mkdir/rmdir cp1254 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1254
--FILE--
<?php
/*
#vim: set fileencoding=cp1254
#vim: set encoding=cp1254
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "çokbaytlı işleri";
$prefix = create_data("dir_cp1254", "{$item}42");
$path = $prefix . DIRECTORY_SEPARATOR . "{$item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "{$item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp1254");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\çokbaytlı işleri42\çokbaytlı işleri4
string(20) "çokbaytlı işleri4"
bool(true)
string(%d) "%s\çokbaytlı işleri42\çokbaytlı işleri4"
Active code page: %d
bool(true)
