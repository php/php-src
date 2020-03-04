--TEST--
Test mkdir/rmdir cp932 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp932
--FILE--
<?php
/*
#vim: set fileencoding=cp932
#vim: set encoding=cp932
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('cp932', 'utf-8', "�e�X�g�}���`�o�C�g�E�p�X"); // cp932 string
$prefix = create_data("dir_cp932", "${item}42");
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp932");

?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\テストマルチバイト・パス42\テストマルチバイト・パス4
string(37) "テストマルチバイト・パス4"
bool(true)
string(%d) "%s\テストマルチバイト・パス42\テストマルチバイト・パス4"
Active code page: %d
bool(true)
===DONE===
