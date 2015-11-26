--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("file");
test_open_basedir_error("file");
     
var_dump(file("ok.txt"));
var_dump(file("../ok/ok.txt"));
var_dump(file($initdir."/test/ok/ok.txt"));
var_dump(file($initdir."/test/ok/../ok/ok.txt"));

test_open_basedir_after("file");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [file] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: file(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d

Warning: file(../bad): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file(../bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d

Warning: file(..): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d

Warning: file(../): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d

Warning: file(/): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d

Warning: file(../bad/.): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file(%s/test/bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: file(): open_basedir restriction in effect. File(%s/test/bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: file(%s/test/bad/../bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)
array(1) {
  [0]=>
  string(12) "Hello World!"
}
array(1) {
  [0]=>
  string(12) "Hello World!"
}
array(1) {
  [0]=>
  string(12) "Hello World!"
}
array(1) {
  [0]=>
  string(12) "Hello World!"
}
*** Finished testing open_basedir configuration [file] ***

