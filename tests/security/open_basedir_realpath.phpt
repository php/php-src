--TEST--
Test open_basedir configuration
--SKIPIF--
<?php
if(PHP_OS_FAMILY !== "Windows") {
    die('skip only run on Windows');
}
?>
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir("realpath");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [realpath] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: realpath(): open_basedir restriction in effect. File(%s\test\bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test\bad\bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test\bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test\bad\bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test\bad\bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: realpath(): open_basedir restriction in effect. File(%s\test) is not within the allowed path(s): (.) in %s on line %d
bool(false)
string(%d) "%s\test\ok"
string(%d) "%s\test\ok\ok.txt"
string(%d) "%s\test\ok\ok.txt"
string(%d) "%s\test\ok\ok.txt"
string(%d) "%s\test\ok\ok.txt"
*** Finished testing open_basedir configuration [realpath] ***
