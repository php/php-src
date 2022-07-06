--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("rename");

var_dump(rename("../bad/bad.txt", "rename.txt"));
var_dump(rename(".././bad/bad.txt", "rename.txt"));
var_dump(rename("../bad/../bad/bad.txt", "rename.txt"));
var_dump(rename("./.././bad/bad.txt", "rename.txt"));
var_dump(rename($initdir."/test/bad/bad.txt", "rename.txt"));

test_open_basedir_after("rename");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [rename] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: rename(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rename(): open_basedir restriction in effect. File(.././bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rename(): open_basedir restriction in effect. File(../bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rename(): open_basedir restriction in effect. File(./.././bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rename(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [rename] ***
