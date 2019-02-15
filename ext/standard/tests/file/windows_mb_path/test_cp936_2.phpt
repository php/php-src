--TEST--
Test fopen() for write cp936 path
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(936, "oem");

?>
--INI--
default_charset=cp936
--FILE--
<?php
/*
#vim: set fileencoding=cp936
#vim: set encoding=cp936
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$item = "測試多字節路徑"; // cp936 string
$prefix = create_data("file_cp936", $item . "25", 936);
$fn = $prefix . DIRECTORY_SEPARATOR . "{$item}25";

$f = fopen($fn, 'w');
if ($f) {
	var_dump($f, fwrite($f, "writing to an mb filename"));
	var_dump(fclose($f));
} else {
	echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 936);

var_dump(unlink($fn));

remove_data("file_cp936");

?>
===DONE===
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 936
getting basename of %s\測試多字節路徑25
string(%d) "測試多字節路徑25"
bool(true)
string(%d) "%s\測試多字節路徑25"
Active code page: %d
bool(true)
===DONE===
