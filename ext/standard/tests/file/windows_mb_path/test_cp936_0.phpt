--TEST--
Test fopen() for reading cp936 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(936, "ansi");

?>
--CONFLICTS--
file_cp936
--INI--
internal_encoding=cp936
--FILE--
<?php
/*
#vim: set fileencoding=cp936
#vim: set encoding=cp936
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "�yԇ���ֹ�·��"; // cp936 string
$prefix = create_data("file_cp936", $item, 936);
$fn = $prefix . DIRECTORY_SEPARATOR . "$item";

$f = fopen($fn, 'r');
if ($f) {
    var_dump($f, fread($f, 42));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

remove_data("file_cp936");

?>
--EXPECTF--
resource(%d) of type (stream)
string(%d) "reading file wihh multibyte filename
"
bool(true)
