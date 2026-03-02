--TEST--
Test session_start() errors
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

try {
    session_start(['option' => new stdClass()]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

$read_and_close = "false";

try {
    session_start([$read_and_close]);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage() . "\n";
}

var_dump(session_start(['option' => false]));

ob_end_flush();

?>
--EXPECTF--
session_start(): Option "option" must be of type string|int|bool, stdClass given
ValueError: session_start(): Argument #1 ($options) must be of type array with keys as string

Warning: session_start(): Setting option "option" failed in %s on line %d
bool(true)
