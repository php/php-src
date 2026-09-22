--TEST--
GH-17541 (ext/session NULL pointer dereferencement during ID reset)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
function errorHandler($errorNumber, $errorMessage, $fileName, $lineNumber) {
    // Destroy session while emitting warning from the bogus session name in session_start
    session_destroy();
}

set_error_handler('errorHandler');

ob_start();
var_dump(session_name("\t"));
var_dump(session_start());

?>
--EXPECTF--
Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d
string(9) "PHPSESSID"
bool(true)
