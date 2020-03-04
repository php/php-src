--TEST--
Test fopen() for reading cp1256 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1256, "ansi");

?>
--CONFLICTS--
file_cp1256
--INI--
internal_encoding=cp1256
--FILE--
<?php
/*
#vim: set fileencoding=cp1256
#vim: set encoding=cp1256
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ"; // cp1256 string
$prefix = create_data("file_cp1256", $item, 1256);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

$f = fopen($fn, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
}

remove_data("file_cp1256");

?>
===DONE===
--EXPECTF--
resource(%d) of type (stream)
string(37) "reading file wihh multibyte filename
"
bool(true)
===DONE===
