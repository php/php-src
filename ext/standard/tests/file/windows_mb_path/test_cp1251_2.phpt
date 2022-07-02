--TEST--
Test fopen() for write CP1251 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1251, "ansi");

?>
--CONFLICTS--
file_cp1251
--INI--
internal_encoding=cp1251
--FILE--
<?php
/*
#vim: set fileencoding=cp1251
#vim: set encoding=cp1251
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "привет"; // cp1251 string
$prefix = create_data("file_cp1251", $item . "7", 1251);
$fn = $prefix . DIRECTORY_SEPARATOR . "{$item}7";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 1251);

var_dump(unlink($fn));
remove_data("file_cp1251");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 1251
getting basename of %s\привет7
string(%d) "привет7"
bool(true)
string(%d) "%s\привет7"
Active code page: %d
bool(true)
