--TEST--
Test mkdir/rmdir eucjp to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php
/*
#vim: set fileencoding=eucjp
#vim: set encoding=eucjp
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("dir_eucjp");
$path = $prefix . DIRECTORY_SEPARATOR . "�ƥ��ȥޥ���Х��ȡ��ѥ�42"; // EUCJP string
$pathw = iconv('eucjp', 'utf-8', $path);

$subpath = $path . DIRECTORY_SEPARATOR . "�ƥ��ȥޥ���Х��ȡ��ѥ�4"; // EUCJP string
$subpathw = iconv('eucjp', 'utf-8', $subpath);

/* The mb dirname exists*/
var_dump(file_exists($pathw));

var_dump(mkdir($subpathw));
var_dump(file_exists($subpathw));

get_basename_with_cp($subpathw, 65001);

var_dump(rmdir($subpathw));
remove_data("dir_eucjp");

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
