--TEST--
Test fopen() for reading cp932 path without conversion 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(932);

?>
--INI--
default_charset=932
--FILE--
<?php
/*
#vim: set fileencoding=cp932
#vim: set encoding=cp932
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("file_eucjp");
$fn = $prefix . DIRECTORY_SEPARATOR . "テストマルチバイト・パス"; // cp932 string

$f = fopen($fn, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open cp932 failed\n";
} 

remove_data("file_eucjp");

?>
===DONE===
--EXPECTF--	
resource(%d) of type (stream)
string(33) "reading file wihh eucjp filename
"
bool(true)
===DONE===
