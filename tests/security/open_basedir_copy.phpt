--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_before("copy");

var_dump(copy("ok.txt", "../bad"));
var_dump(copy("ok.txt", "../bad/bad.txt"));
var_dump(copy("ok.txt", ".."));
var_dump(copy("ok.txt", "../"));
var_dump(copy("ok.txt", "/"));
var_dump(copy("ok.txt", "../bad/."));
var_dump(copy("ok.txt", "../bad/./bad.txt"));
var_dump(copy("ok.txt", "./../."));

var_dump(copy("ok.txt", "copy.txt"));
var_dump(unlink("copy.txt"));
test_open_basedir_after("copy");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [copy] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: copy(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(../bad): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(../bad/bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(..): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(../): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(/): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(../bad/.): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(../bad/./bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(../bad/./bad.txt): failed to open stream: %s in %s on line %d
bool(false)

Warning: copy(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d

Warning: copy(./../.): failed to open stream: %s in %s on line %d
bool(false)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [copy] ***
