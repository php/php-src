--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();

test_open_basedir_before("chmod");

var_dump(chmod("../bad", 0600));
var_dump(chmod("../bad/bad.txt", 0600));
var_dump(chmod("..", 0600));
var_dump(chmod("../", 0600));
var_dump(chmod("/", 0600));
var_dump(chmod("../bad/.", 0600));
var_dump(chmod("../bad/./bad.txt", 0600));
var_dump(chmod("./../.", 0600));

var_dump(chmod($initdir."/test/ok/ok.txt", 0600));
var_dump(chmod("./ok.txt", 0600));
var_dump(chmod("ok.txt", 0600));
var_dump(chmod("../ok/ok.txt", 0600));
var_dump(chmod("../ok/./ok.txt", 0600));
chmod($initdir."/test/ok/ok.txt", 0777);

test_open_basedir_after("chmod");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [chmod] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: chmod(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(../bad/./bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: chmod(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [chmod] ***

