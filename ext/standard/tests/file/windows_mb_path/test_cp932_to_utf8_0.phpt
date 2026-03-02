--TEST--
Test fopen() for reading cp932 to UTF-8 path
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
file_cp932
--FILE--
<?php
/*
#vim: set fileencoding=cp932
#vim: set encoding=cp932
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('cp932', 'utf-8', "テストマルチバイト・パス"); // cp932 string
$prefix = create_data("file_cp932", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

$f = fopen($fn, 'r');
if ($f) {
    var_dump($f, fread($f, 42));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

remove_data("file_cp932");

?>
--EXPECTF--
resource(%d) of type (stream)
string(37) "reading file wihh multibyte filename
"
bool(true)
