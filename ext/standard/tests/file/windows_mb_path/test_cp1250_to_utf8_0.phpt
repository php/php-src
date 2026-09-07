--TEST--
Test fopen() for reading UTF-8 path with cp1250 specific symbols
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
file_cp1250
--FILE--
<?php
/*
#vim: set fileencoding=cp1250
#vim: set encoding=cp1250
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "šđčćž_ŠĐČĆŽ"; // cp1250 specific chars
$prefix = create_data("file_cp1250", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

$f = fopen($fn, 'r');
if ($f) {
    var_dump($f, fread($f, 42));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

remove_data("file_cp1250");

?>
--EXPECTF--
resource(%d) of type (stream)
string(37) "reading file wihh multibyte filename
"
bool(true)
