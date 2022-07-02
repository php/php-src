--TEST--
Test session_set_save_handler() function : error functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : error functionality ***\n";

function callback() { return true; }

try {
    session_set_save_handler("callback", "callback", "callback", "callback", "callback", "callback");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    session_set_save_handler("callback", "echo", "callback", "callback", "callback", "callback");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    session_set_save_handler("callback", "callback", "echo", "callback", "callback", "callback");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    session_set_save_handler("callback", "callback", "callback", "echo", "callback", "callback");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    session_set_save_handler("callback", "callback", "callback", "callback", "echo", "callback");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    session_set_save_handler("callback", "callback", "callback", "callback", "callback", "echo");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

session_set_save_handler("callback", "callback", "callback", "callback", "callback", "callback");

var_dump(session_start());
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : error functionality ***
session_set_save_handler(): Argument #2 ($close) must be a valid callback, function "echo" not found or invalid function name
session_set_save_handler(): Argument #3 ($read) must be a valid callback, function "echo" not found or invalid function name
session_set_save_handler(): Argument #4 ($write) must be a valid callback, function "echo" not found or invalid function name
session_set_save_handler(): Argument #5 ($destroy) must be a valid callback, function "echo" not found or invalid function name
session_set_save_handler(): Argument #6 ($gc) must be a valid callback, function "echo" not found or invalid function name

Warning: session_start(): Failed to read session data: user (%s) in %s on line %d
bool(false)
