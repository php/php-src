--TEST--
Test fopen() for reading CP1251 to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1251
#vim: set encoding=cp1251
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("file_cp1251");
$fn = $prefix . DIRECTORY_SEPARATOR . "привет"; // cp1251 string
$fnw = iconv('cp1251', 'utf-8', $fn);

$f = fopen($fnw, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
} 

remove_data("file_cp1251");

?>
===DONE===
--EXPECTF--	
resource(%d) of type (stream)
string(35) "opened an utf8 filename for reading"
bool(true)
===DONE===
