--TEST--
Test fopen() for reading cp1252 to UTF-8 path
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
file_cp1252
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('cp1252', 'utf-8', "tschüß"); // cp1252 string
$prefix = create_data("file_cp1252", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

$f = fopen($fn, 'r');
if ($f) {
    var_dump($f, fread($f, 42));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

remove_data("file_cp1252");

?>
--EXPECTF--
resource(%d) of type (stream)
string(%d) "hallo
"
bool(true)
