--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("file_put_contents");

var_dump(file_put_contents("../bad/bad.txt", "Hello World!"));
var_dump(file_put_contents(".././bad/bad.txt", "Hello World!"));
var_dump(file_put_contents("../bad/../bad/bad.txt", "Hello World!"));
var_dump(file_put_contents("./.././bad/bad.txt", "Hello World!"));
var_dump(file_put_contents($initdir."/test/bad/bad.txt", "Hello World!"));

test_open_basedir_after("file_put_contents");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [file_put_contents] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: file_put_contents(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_put_contents(../bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: file_put_contents(): open_basedir restriction in effect. File(.././bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_put_contents(.././bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: file_put_contents(): open_basedir restriction in effect. File(../bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_put_contents(../bad/../bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: file_put_contents(): open_basedir restriction in effect. File(./.././bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_put_contents(./.././bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: file_put_contents(): open_basedir restriction in effect. File%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file_put_contents%s/test/bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [file_put_contents] ***
