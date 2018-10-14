--TEST--
Test chdir()/getcwd() with a dir for multibyte filenames
--SKIPIF--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";


skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();


?>
--FILE--
<?php


include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("dir_mb");
$dirw = $prefix . DIRECTORY_SEPARATOR . "テストマルチバイト・パス42";
touch($dirw . DIRECTORY_SEPARATOR . "dummy.txt");

$old_cp = get_active_cp();
set_active_cp(65001);

$oldcwd = getcwd();
var_dump(chdir($dirw));
var_dump(getcwd());

var_dump(file_exists("dummy.txt"));

set_active_cp($old_cp);

chdir($oldcwd);
remove_data("dir_mb");

?>
===DONE===
--EXPECTF--
Active code page: 65001
bool(true)
string(%d) "%s\テストマルチバイト・パス42"
bool(true)
Active code page: %d
===DONE===
