--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_array("lstat");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [lstat] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: lstat(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(%s/test/bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: lstat(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [lstat] ***
