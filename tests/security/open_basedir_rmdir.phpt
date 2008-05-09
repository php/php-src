--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_before("rmdir");
$directory = dirname(__FILE__);

var_dump(rmdir("../bad"));
var_dump(rmdir(".././bad"));
var_dump(rmdir("../bad/../bad"));
var_dump(rmdir("./.././bad"));
var_dump(rmdir($directory."/test/bad"));

test_open_basedir_after("rmdir");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [rmdir] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: rmdir(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rmdir(): open_basedir restriction in effect. File(.././bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rmdir(): open_basedir restriction in effect. File(../bad/../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rmdir(): open_basedir restriction in effect. File(./.././bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: rmdir(): open_basedir restriction in effect. File(%s/test/bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [rmdir] ***

