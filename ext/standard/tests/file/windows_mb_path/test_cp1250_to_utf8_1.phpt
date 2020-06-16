--TEST--
Test mkdir/rmdir UTF-8 path with cp1250 specific symbols
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1250
--FILE--
<?php
/*
#vim: set fileencoding=cp1250
#vim: set encoding=cp1250
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "šđčćž_ŠĐČĆŽ"; // cp1250 specific chars
$prefix = create_data("dir_cp1250", "${item}42");
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp1250");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\šđčćž_ŠĐČĆŽ42\šđčćž_ŠĐČĆŽ4
string(22) "šđčćž_ŠĐČĆŽ4"
bool(true)
string(%d) "%s\šđčćž_ŠĐČĆŽ42\šđčćž_ŠĐČĆŽ4"
Active code page: %d
bool(true)
