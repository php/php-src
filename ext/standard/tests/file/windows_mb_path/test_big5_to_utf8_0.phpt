--TEST--
Test fopen() for reading big5 to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php
/*
#vim: set fileencoding=big5
#vim: set encoding=big5
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$fn = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "測試多字節路徑"; // BIG5 string
$fnw = iconv('big5', 'utf-8', $fn);

$f = fopen($fnw, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
} 

?>
===DONE===
--EXPECTF--	
resource(6) of type (stream)
string(35) "read from a file with big5 filename"
bool(true)
===DONE===
