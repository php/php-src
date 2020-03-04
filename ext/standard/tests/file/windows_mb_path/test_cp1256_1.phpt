--TEST--
Test mkdir/rmdir cp1256 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1256, "ansi");

?>
--CONFLICTS--
dir_cp1256
--INI--
internal_encoding=cp1256
--FILE--
<?php
/*
#vim: set fileencoding=cp1256
#vim: set encoding=cp1256
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ";
$prefix = create_data("dir_cp1256", "${item}42", 1256);
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 1256);

var_dump(rmdir($subpath));
remove_data("dir_cp1256");

?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 1256
getting basename of %s\ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ42\ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ4
string(%d) "ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ4"
bool(true)
string(%d) "%s\ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ42\ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ4"
Active code page: %d
bool(true)
===DONE===
