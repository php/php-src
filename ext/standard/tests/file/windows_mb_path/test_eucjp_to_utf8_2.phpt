--TEST--
Test fopen() for write eucjp to UTF-8 path 
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
#vim: set fileencoding=eucjp
#vim: set encoding=eucjp
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("dir_eucjp");
$fn = $prefix . DIRECTORY_SEPARATOR . "�ƥ��ȥޥ���Х��ȡ��ѥ�33"; // EUCJP string
$fnw = iconv('eucjp', 'utf-8', $fn);

$f = fopen($fnw, 'w');
if ($f) {
	var_dump($f, fwrite($f, "writing to an mb filename"));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
}

var_dump(file_get_contents($fnw));

get_basename_with_cp($fnw, 65001);

remove_data("dir_eucjp");

?>
===DONE===
--EXPECTF--	
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 65001
getting basename of %s\テストマルチバイト・パス33
string(38) "テストマルチバイト・パス33"
bool(true)
string(%d) "%s\テストマルチバイト・パス33"
Active code page: %d
===DONE===
