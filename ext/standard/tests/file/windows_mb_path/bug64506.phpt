--TEST--Bug #64506
PHP can not read or write file correctly if file name have special char like š
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php
/*
#vim: set fileencoding=UTF-8
#vim: set encoding=UTF-8
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$fnw = __DIR__ . DIRECTORY_SEPARATOR . "š.txt"; // UTF-8

$f = fopen($fnw, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
} else {
    echo "open utf8 failed\n";
}
var_dump(fclose($f));

var_dump(file_get_contents($fnw));

get_basename_with_cp($fnw, 65001);

var_dump(unlink($fnw));

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 65001
getting basename of %s\š.txt
string(6) "š.txt"
bool(true)
string(%d) "%s\š.txt"
Active code page: %d
bool(true)
