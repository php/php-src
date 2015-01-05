--TEST--
Test open_basedir configuration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip Windows only variation');
}
?>
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("mkdir");

var_dump(mkdir("../bad/blah"));
var_dump(mkdir("../blah"));
var_dump(mkdir("../bad/./blah"));
var_dump(mkdir("./.././blah"));

var_dump(mkdir($initdir."/test/ok/blah"));
var_dump(rmdir($initdir."/test/ok/blah"));
test_open_basedir_after("mkdir");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [mkdir] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: mkdir(): open_basedir restriction in effect. File(../bad/blah) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: mkdir(): open_basedir restriction in effect. File(../blah) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: mkdir(): open_basedir restriction in effect. File(../bad/./blah) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: mkdir(): open_basedir restriction in effect. File(./.././blah) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [mkdir] ***
