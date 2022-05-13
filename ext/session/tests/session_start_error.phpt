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

var_dump(session_start(['option' => false]));

ob_end_flush();

?>
--EXPECTF--
session_start(): Option "option" must be of type string|int|bool, stdClass given

Warning: session_start(): Setting option "option" failed in %s on line %d
bool(true)
