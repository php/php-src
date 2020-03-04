--TEST--
Test mkdir/rmdir cp1253 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1253, "ansi");

?>
--CONFLICTS--
dir_cp1253
--INI--
default_charset=cp1253
--FILE--
<?php
/*
#vim: set fileencoding=cp1253
#vim: set encoding=cp1253
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "διαδρομή δοκιμής";
$prefix = create_data("dir_cp1253", "${item}42", 1253);
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 1253);

var_dump(rmdir($subpath));
remove_data("dir_cp1253");

?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 1253
getting basename of %s\διαδρομή δοκιμής42\διαδρομή δοκιμής4
string(%d) "διαδρομή δοκιμής4"
bool(true)
string(%d) "%s\διαδρομή δοκιμής42\διαδρομή δοκιμής4"
Active code page: %d
bool(true)
===DONE===
