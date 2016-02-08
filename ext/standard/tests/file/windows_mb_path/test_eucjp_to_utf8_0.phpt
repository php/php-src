--TEST--
Test fopen() for reading eucjp to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php
/*
#vim: set fileencoding=eucjp
#vim: set encoding=eucjp
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("file_eucjp");
$fn = $prefix . DIRECTORY_SEPARATOR . "\テストマルチバイト・パス"; // EUCJP string
$fnw = iconv('eucjp', 'utf-8', $fn);

$f = fopen($fnw, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
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
