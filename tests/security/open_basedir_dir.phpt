--TEST--
Test open_basedir configuration
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();

test_open_basedir_before("dir");
test_open_basedir_error("dir");

var_dump(dir($initdir."/test/ok/"));
var_dump(dir($initdir."/test/ok"));
var_dump(dir($initdir."/test/ok/../ok"));

test_open_basedir_after("dir");?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [dir] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: dir(): open_basedir restriction in effect. File(../bad) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(../bad): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(../bad/bad.txt): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(..): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(../) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(../): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(/) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(/): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(../bad/.) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(../bad/.): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(%s/test/bad/bad.txt): Failed to open directory: %s in %s on line %d
bool(false)

Warning: dir(): open_basedir restriction in effect. File(%s/test/bad/../bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d

Warning: dir(%s/test/bad/../bad/bad.txt): Failed to open directory: %s in %s on line %d
bool(false)
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/test/ok/"
  ["handle"]=>
  resource(%d) of type (stream)
}
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/test/ok"
  ["handle"]=>
  resource(%d) of type (stream)
}
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/test/ok/../ok"
  ["handle"]=>
  resource(%d) of type (stream)
}
*** Finished testing open_basedir configuration [dir] ***
