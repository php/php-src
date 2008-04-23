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
test_open_basedir("glob");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [glob] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
array(0) {
}
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
array(1) {
  [0]=>
  string(5) "../ok"
}
array(1) {
  [0]=>
  string(6) "ok.txt"
}
array(1) {
  [0]=>
  string(12) "../ok/ok.txt"
}
array(1) {
  [0]=>
  string(%d) "%s/test/ok/ok.txt"
}
array(1) {
  [0]=>
  string(%d) "%s/test/ok/../ok/ok.txt"
}
*** Finished testing open_basedir configuration [glob] ***

