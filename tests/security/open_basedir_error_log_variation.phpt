--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("error_log");

define("DESTINATION_IS_FILE", 3);

var_dump(error_log("Hello World!", DESTINATION_IS_FILE, $initdir."/test/bad/bad.txt"));
var_dump(error_log("Hello World!", DESTINATION_IS_FILE, $initdir."/test/bad.txt"));
var_dump(error_log("Hello World!", DESTINATION_IS_FILE, $initdir."/bad.txt"));
var_dump(error_log("Hello World!", DESTINATION_IS_FILE, $initdir."/test/ok/ok.txt"));

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

Warning: error_log(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: error_log(%s/test/bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: error_log(): open_basedir restriction in effect. File(%s/test/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: error_log(%s/test/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: error_log(): open_basedir restriction in effect. File(%s/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: error_log(%s/bad.txt): failed to open stream: %s in %s on line %d
bool(false)
bool(true)
*** Finished testing open_basedir configuration [error_log] ***
