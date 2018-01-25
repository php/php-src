--TEST--
Test fopen() for reading big5 path
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(950, "ansi");

?>
--INI--
defalut_charset=big5
--FILE--
<?php
/*
#vim: set fileencoding=big5
#vim: set encoding=big5
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$item = "測試多字節路徑"; // BIG5 string
$prefix = create_data("file_big5", $item, 950);
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
===DONE===
--EXPECTF--
resource(%d) of type (stream)
string(%d) "reading file wihh multibyte filename
"
bool(true)
===DONE===
