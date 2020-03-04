--TEST--
Test mkdir/rmdir cp1256 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1256
--FILE--
<?php
/*
#vim: set fileencoding=cp1256
#vim: set encoding=cp1256
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "مسار متعدد البايت اختبار";
$prefix = create_data("dir_cp1256", "${item}42");
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp1256");

?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\مسار متعدد البايت اختبار42\مسار متعدد البايت اختبار4
string(46) "مسار متعدد البايت اختبار4"
bool(true)
string(%d) "%s\مسار متعدد البايت اختبار42\مسار متعدد البايت اختبار4"
Active code page: %d
bool(true)
===DONE===
