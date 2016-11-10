--TEST--
Bug #60634 (Segmentation fault when trying to die() in SessionHandler::write())
--INI--
session.save_path=
session.name=PHPSESSID
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
 * This test raises error in Unknown function because 2nd close write handler is
 * called at request shutdown and session module detects recursive call like
 * multiple save handler calls.
 */

?>
--EXPECTF--
write: goodbye cruel world

Warning: Unknown: Cannot call save handler function recursive manner in Unknown on line 0

Warning: Unknown: Failed to write session data using user defined save handler. (session.save_path: ) in Unknown on line 0
close: goodbye cruel world
