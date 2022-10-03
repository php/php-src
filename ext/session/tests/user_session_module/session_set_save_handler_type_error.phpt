--TEST--
Test session_set_save_handler() function: interface wrong
--EXTENSIONS--
session
--FILE--
<?php

$validCallback = function () { return true; };
$deprecatedCallback = function () { return 0; };
$exceptionCallback = function () { return []; };

ob_start();

try {
    $ret = session_set_save_handler($exceptionCallback, $validCallback, $validCallback, $validCallback, $validCallback, $validCallback);
    session_start();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $ret = session_set_save_handler($deprecatedCallback, $validCallback, $validCallback, $validCallback, $validCallback, $validCallback);
    session_start();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $ret = session_set_save_handler($validCallback, $exceptionCallback, $validCallback, $validCallback, $validCallback, $validCallback);
    session_start();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $ret = session_set_save_handler($validCallback, $deprecatedCallback, $exceptionCallback, $validCallback, $validCallback, $validCallback);
    session_start();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

ob_end_flush();

?>
--EXPECTF--
Session callback must have a return value of type bool, array returned

Deprecated: session_start(): Session callback must have a return value of type bool, int returned in %s on line %d

Warning: session_start(): Failed to read session data: user (%s) in %s on line %d
Session callback must have a return value of type bool, array returned

Deprecated: session_start(): Session callback must have a return value of type bool, int returned in %s on line %d

Warning: session_start(): Failed to read session data: user (%s) in %s on line %d
