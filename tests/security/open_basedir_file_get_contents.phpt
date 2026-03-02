--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("file_get_contents");
test_open_basedir_error("file_get_contents");

var_dump(file_get_contents("ok.txt"));
var_dump(file_get_contents("../ok/ok.txt"));
var_dump(file_get_contents($initdir."/test/ok/ok.txt"));
var_dump(file_get_contents($initdir."/test/ok/../ok/ok.txt"));

test_open_basedir_after("file_get_contents");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [file_get_contents] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: file_get_contents(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(../bad): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(../bad/bad.txt): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(..): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(../): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(/): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(../bad/.): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(%s/test/bad/bad.txt): Failed to open stream: %s in %s on line %d
bool(false)

Warning: file_get_contents(): open_basedir restriction in effect. File(%s/test/bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_get_contents(%s/test/bad/../bad/bad.txt): Failed to open stream: %s in %s on line %d
bool(false)
string(12) "Hello World!"
string(12) "Hello World!"
string(12) "Hello World!"
string(12) "Hello World!"
*** Finished testing open_basedir configuration [file_get_contents] ***
