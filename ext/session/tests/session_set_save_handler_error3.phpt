--TEST--
Test session_set_save_handler() function : error functionality
--INI--
session.save_path=
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_set_save_handler(callback $open, callback $close, callback $read, callback $write, callback $destroy, callback $gc)
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c 
 */

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

Warning: Uncaught Exception: Do something bad..! in %s:%d
Stack trace:
#0 [internal function]: open('', 'PHPSESSID')
#1 %s(%d): session_start()
#2 {main}
  thrown in %s on line %d

Fatal error: session_start(): Failed to initialize storage module: %s in %ssession_set_save_handler_error3.php on line %d
