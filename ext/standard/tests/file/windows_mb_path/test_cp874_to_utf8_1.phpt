--TEST--
Thai UTF-8 cmd test
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
file_cp874
--FILE--
<?php
/*
#vim: set fileencoding=cp874
#vim: set encoding=cp874
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";


$item = "เป็นแฟ้มที่ทดสอบ11";
$prefix = create_data("file_cp874", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b " . $fn);

remove_data("file_cp874");

?>
--EXPECTF--
string(%d) "%s\เป็นแฟ้มที่ทดสอบ11"
bool(true)
bool(true)
เป็นแฟ้มที่ทดสอบ11
