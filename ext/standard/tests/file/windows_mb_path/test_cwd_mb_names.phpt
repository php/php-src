--TEST--
Test chdir()/getcwd() with a dir for multibyte filenames

--SKIPIF--

<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";


skip_if_no_required_exts();
skip_if_not_win();


?>

--FILE--

<?php


include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";



$dirw = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "テストマルチバイト・パス42";



$old_cp = get_active_cp();

set_active_cp(65001);






var_dump(chdir($dirw));
var_dump(getcwd());

var_dump(file_exists("dummy.txt"));

set_active_cp($old_cp);


?>
===DONE===

--EXPECTF--
Active code page: 65001
bool(true)
string(%d) "%s\テストマルチバイト・パス42"
bool(true)
Active code page: 437
===DONE===
