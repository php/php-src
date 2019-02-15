--TEST--
cp1254 cmd test
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1254
#vim: set encoding=cp1254
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";


$item = "çokbaytlı işleri";
$prefix = create_data("file_cp1254", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b \"" . $fn . "\"");

remove_data("file_cp1254");

?>
===DONE===
--EXPECTF--
string(%d) "%s\çokbaytlı işleri"
bool(true)
bool(true)
çokbaytlı işleri
===DONE===
