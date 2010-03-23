--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir("file_exists");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [file_exists] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: file_exists(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(%s/test/bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: file_exists(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [file_exists] ***

