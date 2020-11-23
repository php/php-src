--TEST--
Test fopen() for reading CP1251 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1251, "ansi");

?>
--CONFLICTS--
file_cp1251
--INI--
default_charset=cp1251
--FILE--
<?php
/*
#vim: set fileencoding=cp1251
#vim: set encoding=cp1251
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "привет"; // cp1251 string
$prefix = create_data("file_cp1251", $item, 1251);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

$f = fopen($fn, 'r');
if ($f) {
    var_dump($f, fread($f, 42));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

remove_data("file_cp1251");

?>
--EXPECTF--
resource(%d) of type (stream)
string(35) "opened an utf8 filename for reading"
bool(true)
