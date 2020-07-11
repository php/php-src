--TEST--
Test mkdir/rmdir cp1255 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1255, "ansi");

?>
--CONFLICTS--
dir_cp1255
--INI--
default_charset=cp1255
--FILE--
<?php
/*
#vim: set fileencoding=cp1255
#vim: set encoding=cp1255
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "כללים מרובים";
$prefix = create_data("dir_cp1255", "${item}42", 1255);
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 1255);

var_dump(rmdir($subpath));
remove_data("dir_cp1255");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 1255
getting basename of %s\כללים מרובים42\כללים מרובים4
string(%d) "כללים מרובים4"
bool(true)
string(%d) "%s\כללים מרובים42\כללים מרובים4"
Active code page: %d
bool(true)
