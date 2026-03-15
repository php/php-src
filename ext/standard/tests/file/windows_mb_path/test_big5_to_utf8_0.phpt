--TEST--
Test fopen() for reading big5 to UTF-8 path
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
file_big5
--FILE--
<?php
/*
#vim: set fileencoding=big5
#vim: set encoding=big5
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('big5', 'utf-8', "測試多字節路徑"); // BIG5 string
$prefix = create_data("file_big5", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . "$item";

$f = fopen($fn, 'r');
if ($f) {
    var_dump($f, fread($f, 42));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

remove_data("file_big5");

?>
--EXPECTF--
resource(%d) of type (stream)
string(%d) "reading file wihh multibyte filename
"
bool(true)
