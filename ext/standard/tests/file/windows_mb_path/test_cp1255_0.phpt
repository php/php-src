--TEST--
Test fopen() for reading cp1255 path
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1255, "ansi");

?>
--INI--
internal_encoding=cp1255
--FILE--
<?php
/*
#vim: set fileencoding=cp1255
#vim: set encoding=cp1255
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$item = "כללים מרובים";
$prefix = create_data("file_cp1255", $item, 1255);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

$f = fopen($fn, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
}

remove_data("file_cp1255");

?>
===DONE===
--EXPECTF--
resource(%d) of type (stream)
string(37) "reading file wihh multibyte filename
"
bool(true)
===DONE===
