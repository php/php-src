--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_before("copy");

var_dump(copy("../bad/bad.txt", "copy.txt"));
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

Warning: copy(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: unlink(copy.txt): No such file or directory in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [copy] ***
