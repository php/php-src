--TEST--
Test fopen() for write cp1256 to UTF-8 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(1256, "ansi");

?>
--CONFLICTS--
dir_cp1256
--INI--
default_charset=cp1256
--FILE--
<?php
/*
#vim: set fileencoding=cp1256
#vim: set encoding=cp1256
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ";
$prefix = create_data("dir_cp1256", "${item}42}", 1256);
$fn = $prefix . DIRECTORY_SEPARATOR . "${item}33";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 1256);

remove_data("dir_cp1256");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 1256
getting basename of %s\ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ33
string(%d) "ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ33"
bool(true)
string(%d) "%s\ãÓÇÑ ãÊÚÏÏ ÇáÈÇíÊ ÇÎÊÈÇÑ33"
Active code page: %s
