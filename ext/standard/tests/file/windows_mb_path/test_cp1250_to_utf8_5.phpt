--TEST--
Test fopen() for write to UTF-8 path with cp1250 specific symbols
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
dir_cp1250
--FILE--
<?php
/*
#vim: set fileencoding=cp1250
#vim: set encoding=cp1250
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = "Árvíztűrő tükörfúrógép"; // cp1250 specific chars
$prefix = create_data("dir_cp1250", "${item}42}");
$fn = $prefix . DIRECTORY_SEPARATOR . "${item}33";

$f = fopen($fn, 'w');
if ($f) {
    var_dump($f, fwrite($f, "writing to an mb filename"));
    var_dump(fclose($f));
} else {
    echo "open utf8 failed\n";
}

var_dump(file_get_contents($fn));

get_basename_with_cp($fn, 65001);

remove_data("dir_cp1250");

?>
--EXPECTF--
resource(%d) of type (stream)
int(25)
bool(true)
string(25) "writing to an mb filename"
Active code page: 65001
getting basename of %s\Árvíztűrő tükörfúrógép33
string(33) "Árvíztűrő tükörfúrógép33"
bool(true)
string(%d) "%s\Árvíztűrő tükörfúrógép33"
Active code page: %d
