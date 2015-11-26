--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
error_log=
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("error_log");


var_dump(ini_set("error_log", $initdir."/test/bad/bad.txt"));
var_dump(ini_set("error_log", $initdir."/test/bad.txt"));
var_dump(ini_set("error_log", $initdir."/bad.txt"));
var_dump(ini_set("error_log", $initdir."/test/ok/ok.txt"));
var_dump(ini_set("error_log", $initdir."/test/ok/ok.txt"));

test_open_basedir_after("error_log");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [error_log] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: ini_set(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: ini_set(): open_basedir restriction in effect. File(%s/test/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: ini_set(): open_basedir restriction in effect. File(%s/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
string(0) ""
string(%d) "%s/test/ok/ok.txt"
*** Finished testing open_basedir configuration [error_log] ***

