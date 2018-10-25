--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--SKIPIF--
<?php
if(PHP_OS_FAMILY !== "Windows") {
    die('skip Windows only variation');
}
?>
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_before("parse_ini_file");
$directory = dirname(__FILE__);

var_dump(parse_ini_file("../bad"));
var_dump(parse_ini_file("../bad/bad.txt"));
var_dump(parse_ini_file(".."));
var_dump(parse_ini_file("../"));
var_dump(parse_ini_file("../bad/."));
var_dump(parse_ini_file("../bad/./bad.txt"));
var_dump(parse_ini_file("./../."));

test_open_basedir_after("parse_ini_file");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [parse_ini_file] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest\bad) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 6

Warning: parse_ini_file(%stest\bad): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 6
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest\bad\bad.txt) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 7

Warning: parse_ini_file(%stest\bad\bad.txt): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 7
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 8

Warning: parse_ini_file(%stest): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 8
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 9

Warning: parse_ini_file(%stest): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 9
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest\bad) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 10

Warning: parse_ini_file(%stest\bad): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 10
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest\bad\bad.txt) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 11

Warning: parse_ini_file(%stest\bad\bad.txt): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 11
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(%stest) is not within the allowed path(s): (.) in %sopen_basedir_parse_ini_file.php on line 12

Warning: parse_ini_file(%stest): failed to open stream: Operation not permitted in %sopen_basedir_parse_ini_file.php on line 12
bool(false)
*** Finished testing open_basedir configuration [parse_ini_file] ***
