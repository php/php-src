--TEST--
Test session_set_cookie_params() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_cookie_params() : basic functionality ***\n";

var_dump(session_set_cookie_params(3600));
var_dump(session_start());
var_dump(session_set_cookie_params(1800));
var_dump(session_destroy());
var_dump(session_set_cookie_params(1234567890));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : basic functionality ***
bool(true)
bool(true)

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
bool(true)
bool(true)
Done
