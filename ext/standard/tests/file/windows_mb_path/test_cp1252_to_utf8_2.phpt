--TEST--
Test fopen() for write cp1252 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1252
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv('cp1252', 'utf-8', "tsch��"); // cp1252 string
$prefix = create_data("dir_cp1252", "${item}3");
$fn = $prefix . DIRECTORY_SEPARATOR . "${item}7";

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
remove_data("dir_cp1252");

?>
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
