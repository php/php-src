--TEST--
Test fopen() for write to cp932 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(932, "oem");

?>
--CONFLICTS--
dir_cp932
--INI--
internal_encoding=cp932
--FILE--
<?php
/*
#vim: set fileencoding=cp932
#vim: set encoding=cp932
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "テストマルチバイト・パス"; // cp932 string
$prefix = create_data("dir_cp932", "{$item}42}", 932);
$fn = $prefix . DIRECTORY_SEPARATOR . "{$item}33";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 932);

remove_data("dir_cp932");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 932
getting basename of %s\テストマルチバイト・パス33
string(%d) "テストマルチバイト・パス33"
bool(true)
string(%d) "%s\テストマルチバイト・パス33"
Active code page: %d
