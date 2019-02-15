--TEST--
Test mkdir/rmdir UTF-8 path with cp1250 specific symbols
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1250
#vim: set encoding=cp1250
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$item = "Árvíztűrő tükörfúrógép"; // cp1250 specific chars
$prefix = create_data("dir_cp1250", "${item}42");
$path = $prefix . DIRECTORY_SEPARATOR . "${item}42";

$subpath = $path . DIRECTORY_SEPARATOR . "${item}4";

/* The mb dirname exists*/
var_dump(file_exists($path));

var_dump(mkdir($subpath));
var_dump(file_exists($subpath));

get_basename_with_cp($subpath, 65001);

var_dump(rmdir($subpath));
remove_data("dir_cp1250");

?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\Árvíztűrő tükörfúrógép42\Árvíztűrő tükörfúrógép4
string(32) "Árvíztűrő tükörfúrógép4"
bool(true)
string(%d) "%s\Árvíztűrő tükörfúrógép42\Árvíztűrő tükörfúrógép4"
Active code page: %d
bool(true)
===DONE===
