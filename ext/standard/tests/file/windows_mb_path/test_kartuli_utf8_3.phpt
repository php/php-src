--TEST--
Kartuli UTF-8 cmd test
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--CONFLICTS--
file_kartuli
--FILE--
<?php
/*
#vim: set fileencoding=cp874
#vim: set encoding=cp874
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";


$item = "ქართველები55";
$prefix = create_data("file_kartuli", $item);
$fn = $prefix . DIRECTORY_SEPARATOR . $item;

var_dump($fn);
var_dump(touch($fn));
var_dump(file_exists($fn));
system("dir /b " . $fn);

remove_data("file_kartuli");

?>
--EXPECTF--
string(%d) "%s\ქართველები55"
bool(true)
bool(true)
ქართველები55
