--TEST--
Bug #60634 (Segmentation fault when trying to die() in SessionHandler::write()) - exception in write during exec
--XFAIL--
Long term low priority bug, working on it
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
	echo "close: goodbye cruel world\n";
}

function read($id) {
	return '';
}

function write($id, $session_data) {
	echo "write: goodbye cruel world\n";
	throw new Exception;
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

?>
--EXPECTF--
write: goodbye cruel world

Fatal error: Uncaught exception 'Exception' in %s
