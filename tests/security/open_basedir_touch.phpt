--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir_before("touch");
$directory = dirname(__FILE__);

var_dump(touch("../bad"));
var_dump(touch("../bad/bad.txt"));
var_dump(touch(".."));
var_dump(touch("../"));
var_dump(touch("/"));
var_dump(touch("../bad/."));
var_dump(touch("../bad/./bad.txt"));
var_dump(touch("./../."));

var_dump(touch($directory."/test/ok/ok.txt"));
var_dump(touch("./ok.txt"));
var_dump(touch("ok.txt"));
var_dump(touch("../ok/ok.txt"));
var_dump(touch("../ok/./ok.txt"));

test_open_basedir_after("touch");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [touch] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: touch(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(../bad/./bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: touch(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [touch] ***

