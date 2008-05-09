--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_before("tempnam");
$directory = dirname(__FILE__);

var_dump(tempnam("../bad", "test"));
var_dump(tempnam("..", "test"));
var_dump(tempnam("../", "test"));
var_dump(tempnam("/", "test"));
var_dump(tempnam("../bad/.", "test"));
var_dump(tempnam("./../.", "test"));

$file = tempnam($directory."/test/ok", "test");
var_dump($file);
var_dump(unlink($file));

test_open_basedir_after("tempnam");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [tempnam] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: tempnam(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: tempnam(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: tempnam(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: tempnam(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: tempnam(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: tempnam(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d
bool(false)
string(%d) "%s"
bool(true)
*** Finished testing open_basedir configuration [tempnam] ***

