--TEST--
Test fopen() for write cp1254 path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(857, "oem");

?>
--CONFLICTS--
dir_cp1254
--INI--
internal_encoding=cp1254
--FILE--
<?php
/*
#vim: set fileencoding=cp1254
#vim: set encoding=cp1254
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "çokbaytlý iþleri";
$prefix = create_data("dir_cp1254", "${item}42}", 1254);
$fn = $prefix . DIRECTORY_SEPARATOR . "${item}33";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 1254);

remove_data("dir_cp1254");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 1254
getting basename of %s\çokbaytlý iþleri33
string(%d) "çokbaytlý iþleri33"
bool(true)
string(%d) "%s\çokbaytlý iþleri33"
Active code page: %d
