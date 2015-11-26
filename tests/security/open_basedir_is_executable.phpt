--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("is_executable");
test_open_basedir_error("is_executable");     

var_dump(is_executable("ok.txt"));
var_dump(is_executable("../ok/ok.txt"));
var_dump(is_executable($initdir."/test/ok/ok.txt"));
var_dump(is_executable($initdir."/test/ok/../ok/ok.txt"));

test_open_basedir_after("is_executable");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [is_executable] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: is_executable(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: is_executable(): open_basedir restriction in effect. File(%s/test/bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
*** Finished testing open_basedir configuration [is_executable] ***
