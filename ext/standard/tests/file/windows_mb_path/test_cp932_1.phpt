--TEST--
Test mkdir/rmdir cp932
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(932, "oem");

?>
--CONFLICTS--
dir_cp932
--INI--
default_charset=cp932
--FILE--
<?php
/*
#vim: set fileencoding=cp932
#vim: set encoding=cp932
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "�e�X�g�}���`�o�C�g�E�p�X"; // cp932 string
$prefix = create_data("dir_cp932", "${item}42", 932);
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 932);

var_dump(rmdir($subpath));
remove_data("dir_cp932");

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 932
getting basename of %s\�e�X�g�}���`�o�C�g�E�p�X42\�e�X�g�}���`�o�C�g�E�p�X4
string(%d) "�e�X�g�}���`�o�C�g�E�p�X4"
bool(true)
string(%d) "%s\�e�X�g�}���`�o�C�g�E�p�X42\�e�X�g�}���`�o�C�g�E�p�X4"
Active code page: %d
bool(true)
