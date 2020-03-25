--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("unlink");

var_dump(unlink("../bad/bad.txt"));
var_dump(unlink(".././bad/bad.txt"));
var_dump(unlink("../bad/../bad/bad.txt"));
var_dump(unlink("./.././bad/bad.txt"));
var_dump(unlink($initdir."/test/bad/bad.txt"));

test_open_basedir_after("unlink");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [unlink] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: unlink(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: unlink(): open_basedir restriction in effect. File(.././bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: unlink(): open_basedir restriction in effect. File(../bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: unlink(): open_basedir restriction in effect. File(./.././bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: unlink(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [unlink] ***
