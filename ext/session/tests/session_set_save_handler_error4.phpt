--TEST--
Test session_set_save_handler() function : error functionality
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

function callback() { return true; }

session_set_save_handler("callback", "callback", "callback", "callback", "callback", "callback");
session_set_save_handler("callback", "echo", "callback", "callback", "callback", "callback");
session_set_save_handler("callback", "callback", "echo", "callback", "callback", "callback");
session_set_save_handler("callback", "callback", "callback", "echo", "callback", "callback");
session_set_save_handler("callback", "callback", "callback", "callback", "echo", "callback");
session_set_save_handler("callback", "callback", "callback", "callback", "callback", "echo");
session_set_save_handler("callback", "callback", "callback", "callback", "callback", "callback");
session_start();
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : error functionality ***

Warning: session_set_save_handler(): Argument 2 is not a valid callback in %s on line %d

Warning: session_set_save_handler(): Argument 3 is not a valid callback in %s on line %d

Warning: session_set_save_handler(): Argument 4 is not a valid callback in %s on line %d

Warning: session_set_save_handler(): Argument 5 is not a valid callback in %s on line %d

Warning: session_set_save_handler(): Argument 6 is not a valid callback in %s on line %d
