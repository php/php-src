--TEST--
Test session_set_save_handler() function : error functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
error_reporting=0
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : error functionality ***\n";

function open($save_path, $session_name) { return true; }
function close() { return true; }
function read($id) { return false; }
function write($id, $session_data) { }
function destroy($id) {  return true; }
function gc($maxlifetime) {  return true; }

session_set_save_handler("open", "close", "read", "write", "destroy", "gc");

session_start();
$_SESSION["Blah"] = "Hello World!";
$_SESSION["Foo"] = FALSE;
$_SESSION["Guff"] = 1234567890;
var_dump($_SESSION);

session_write_close();
var_dump($_SESSION);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
session_start();
var_dump($_SESSION);
session_destroy();

ob_end_flush();
?>
--EXPECT--
*** Testing session_set_save_handler() : error functionality ***
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
array(0) {
}
