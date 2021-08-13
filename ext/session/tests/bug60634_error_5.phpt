--TEST--
Bug #60634 (Segmentation fault when trying to die() in SessionHandler::write()) - fatal error in close during exec
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--EXTENSIONS--
session
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
    undefined_function();
}

function read($id) {
    return '';
}

function write($id, $session_data) {
    return true;
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
close: goodbye cruel world

Fatal error: Uncaught Error: Call to undefined function undefined_function() in %s:%d
Stack trace:
#0 [internal function]: close()
#1 %s(%d): session_write_close()
#2 {main}
  thrown in %s on line %d
