--TEST--
Test fopen() for write big5 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
file_big5
--FILE--
<?php
/*
#vim: set fileencoding=big5
#vim: set encoding=big5
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('big5', 'utf-8', "���զh�r�`���|"); // BIG5 string
$prefix = create_data("file_big5", $item . "25");
$fn = $prefix . DIRECTORY_SEPARATOR . "{$item}25";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 65001);

var_dump(unlink($fn));

remove_data("file_big5");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 65001
getting basename of %s\測試多字節路徑25
string(23) "測試多字節路徑25"
bool(true)
string(%d) "%s\測試多字節路徑25"
Active code page: %d
bool(true)
