--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
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

Warning: parse_ini_file(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d

Warning: parse_ini_file(../bad): failed to open stream: Operation not permitted in %s on line %d
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: parse_ini_file(../bad/bad.txt): failed to open stream: Operation not permitted in %s on line %d
bool(false)

Warning: parse_ini_file(..): failed to open stream: Operation not permitted in %s on line %d
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d

Warning: parse_ini_file(../): failed to open stream: Operation not permitted in %s on line %d
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d

Warning: parse_ini_file(../bad/.): failed to open stream: Operation not permitted in %s on line %d
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(../bad/./bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: parse_ini_file(../bad/./bad.txt): failed to open stream: Operation not permitted in %s on line %d
bool(false)

Warning: parse_ini_file(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d

Warning: parse_ini_file(./../.): failed to open stream: Operation not permitted in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [parse_ini_file] ***

