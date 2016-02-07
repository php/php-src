--TEST--
Test fopen() for reading cp1252 to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("file2_cp1252");
$fn = $prefix . DIRECTORY_SEPARATOR . "Voláçao"; // cp1252 string
$fnw = iconv('cp1252', 'utf-8', $fn);

$f = fopen($fnw, 'r');
if ($f) {
	var_dump($f, fread($f, 42));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
} 

remove_data("file2_cp1252");

?>
===DONE===
--EXPECTF--	
resource(%d) of type (stream)
string(4) "hola"
bool(true)
===DONE===
