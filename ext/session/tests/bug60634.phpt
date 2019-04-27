--TEST--
Bug #60634 (Segmentation fault when trying to die() in SessionHandler::write())
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

function open($save_path, $session_name) {
    return true;
}

function close() {
	die("close: goodbye cruel world\n");
}

function read($id) {
	return '';
}

function write($id, $session_data) {
	die("write: goodbye cruel world\n");
}

function destroy($id) {
    return true;
}

function gc($maxlifetime) {
    return true;
}

session_set_save_handler('open', 'close', 'read', 'write', 'destroy', 'gc');
session_start();
session_write_close();
echo "um, hi\n";

/*
 * write() calls die(). This results in calling session_flush() which calls
 * write() in request shutdown. The code is inside save handler still and
 * calls save close handler.
 *
 * Because session_write_close() fails by die(), write() is called twice.
 * close() is still called at request shutdown since session is active.
 */

?>
--EXPECT--
write: goodbye cruel world

Warning: Unknown: Cannot call session save handler in a recursive manner in Unknown on line 0

Warning: Unknown: Failed to write session data using user defined save handler. (session.save_path: ) in Unknown on line 0
close: goodbye cruel world
