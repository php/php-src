--TEST--
Thai UTF-8 cmd test
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
#vim: set fileencoding=cp874
#vim: set encoding=cp874
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";


$item = "เป็นแฟ้มที่ทดสอบ11";
$prefix = create_data("file_cp874", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b " . $fn);

remove_data("file_cp874");

?>
===DONE===
--EXPECTF--
string(%d) "%s\เป็นแฟ้มที่ทดสอบ11"
bool(true)
bool(true)
เป็นแฟ้มที่ทดสอบ11
===DONE===
