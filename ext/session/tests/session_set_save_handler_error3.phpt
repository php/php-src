--TEST--
Test session_set_save_handler() function : error functionality
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : error functionality ***\n";
function open($save_path, $session_name) {
     throw new Exception("Do something bad..!");
}

function close() { return true; }
function read($id) { return false; }
function write($id, $session_data) { }
function destroy($id) {  return true; }
function gc($maxlifetime) {  return true; }

session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
session_start();
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : error functionality ***

Fatal error: Uncaught Exception: Do something bad..! in %s:%d
Stack trace:
#0 [internal function]: open('', 'PHPSESSID')
#1 %s(%d): session_start()
#2 {main}
  thrown in %s on line %d
