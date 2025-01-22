--TEST--
GH-17541 (ext/session NULL pointer dereferencement during ID reset)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
if (defined("pass3")) {
} else {
function errorHandler($errorNumber, $errorMessage, $fileName, $lineNumber) {
define("pass3", 1);
include(__FILE__);
}
set_error_handler('errorHandler');
}
ob_start();
var_dump(session_name("\t"));
var_dump(session_start());
var_dump(session_destroy());

?>
--EXPECTF--
Warning: session_name(): session.name "	" must not be numeric, empty, or contain any of the following characters "=,;.[ \t\r\n\013\014" in %s on line %d
string(9) "PHPSESSID"
bool(true)
bool(true)
string(9) "PHPSESSID"
bool(true)
bool(true)
