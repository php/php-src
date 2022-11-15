--TEST--
Test session_set_save_handler() function: Incorrect bool returns
--EXTENSIONS--
session
--FILE--
<?php

$validCallback = function () { return true; };
$nullCallback = function () { return; };
$oneCallback = function () { return 1; };

ob_start();

try {
    $ret = session_set_save_handler($nullCallback, $validCallback, $validCallback, $validCallback, $validCallback, $validCallback);
    session_start();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    $ret = session_set_save_handler($oneCallback, $validCallback, $validCallback, $validCallback, $validCallback, $validCallback);
    session_start();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

ob_end_flush();

?>
--EXPECT--
Session callback must have a return value of type bool, null returned
Session callback must have a return value of type bool, int returned
