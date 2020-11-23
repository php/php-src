--TEST--
cp1252 cmd test
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(437, "oem");

?>
--CONFLICTS--
file
--INI--
internal_encoding=cp1252
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "gef‰ﬂ";
$prefix = create_data("file", $item, 1252);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b " . $fn);

remove_data("file");

?>
--EXPECTF--
string(%d) "%s\gef‰ﬂ"
bool(true)
bool(true)
gef‰ﬂ
