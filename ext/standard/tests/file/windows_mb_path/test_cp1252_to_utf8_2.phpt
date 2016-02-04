--TEST--
Test fopen() for write cp1252 to UTF-8 path 
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

$fn = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "tsch��7"; // cp1252 string
$fnw = iconv('cp1252', 'utf-8', $fn);

$f = fopen($fnw, 'w');
if ($f) {
	var_dump($f, fwrite($f, "writing to an mb filename"));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
}

var_dump(file_get_contents($fnw));

get_basename_with_cp($fnw, 65001);

var_dump(unlink($fnw));

?>
===DONE===
--EXPECTF--	
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 65001
getting basename of %s\tschüß7
string(9) "tschüß7"
bool(true)
string(%d) "%s\tschüß7"
Active code page: %d
bool(true)
===DONE===
