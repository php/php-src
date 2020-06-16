--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("fopen");

var_dump(fopen("../bad", "r"));
var_dump(fopen("../bad/bad.txt", "r"));
var_dump(fopen("..", "r"));
var_dump(fopen("../", "r"));
var_dump(fopen("/", "r"));
var_dump(fopen("../bad/.", "r"));
var_dump(fopen("../bad/./bad.txt", "r"));
var_dump(fopen("./../.", "r"));

var_dump(fopen($initdir."/test/ok/ok.txt", "r"));
var_dump(fopen("./ok.txt", "r"));
var_dump(fopen("ok.txt", "r"));
var_dump(fopen("../ok/ok.txt", "r"));
var_dump(fopen("../ok/./ok.txt", "r"));

test_open_basedir_after("fopen");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [fopen] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: fopen(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(../bad): Failed to open stream: %s in %s on line %d
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(../bad/bad.txt): Failed to open stream: %s in %s on line %d
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(..): Failed to open stream: %s in %s on line %d
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(../): Failed to open stream: %s in %s on line %d
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(/): Failed to open stream: %s in %s on line %d
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(../bad/.): Failed to open stream: %s in %s on line %d
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(../bad/./bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(../bad/./bad.txt): Failed to open stream: %s in %s on line 12
bool(false)

Warning: fopen(): open_basedir restriction in effect. File(./../.) is not within the allowed path(s): (.) in %s on line %d

Warning: fopen(./../.): Failed to open stream: %s in %s on line %d
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
*** Finished testing open_basedir configuration [fopen] ***
