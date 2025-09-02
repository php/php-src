--TEST--
Test fopen() for write cp1255 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1255, "ansi");

?>
--CONFLICTS--
dir_cp1255
--INI--
internal_encoding=cp1255
--FILE--
<?php
/*
#vim: set fileencoding=cp1255
#vim: set encoding=cp1255
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "כללים מרובים";
$prefix = create_data("dir_cp1255", "{$item}42}", 1255);
$fn = $prefix . DIRECTORY_SEPARATOR . "{$item}33";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 1255);

remove_data("dir_cp1255");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 1255
getting basename of %s\כללים מרובים33
string(%d) "כללים מרובים33"
bool(true)
string(%d) "%s\כללים מרובים33"
Active code page: %d
