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
FIXME: Since session module try to write/close session data in
RSHUTDOWN, write() is executed twices. This is caused by undefined
function error and zend_bailout(). Current session module codes
depends on this behavior. These codes should be modified to remove
multiple write().
*/

?>
--EXPECTF--
write: goodbye cruel world
write: goodbye cruel world
close: goodbye cruel world

