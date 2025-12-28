--TEST--
Test session_start() with flag read_and_close
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

$valuesEnablingReadAndClose = [true, 1, "1", "777", 777, -1];

$valuesDisablingReadAndClose = ["true", false, "false", 0, "0", "no", "00000"];

foreach ($valuesEnablingReadAndClose as $value) {
    session_start(["read_and_close" => $value]);
    var_dump(session_status() === PHP_SESSION_NONE);
}

foreach ($valuesDisablingReadAndClose as $value) {
    try {
    	session_start(["read_and_close" => $value]);
    } catch (TypeError $e) {
    	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    var_dump(session_status() === PHP_SESSION_ACTIVE);
    session_write_close();
}

try {
    session_start(["read_and_close" => 1.0]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

ob_end_flush();
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)

Notice: session_start(): Ignoring session_start() because a session is already active (started from %s on line %d) in %s on line %d
bool(true)
bool(true)
TypeError: session_start(): Option "read_and_close" value must be of type compatible with int, "false" given
bool(false)
bool(true)
bool(true)
TypeError: session_start(): Option "read_and_close" value must be of type compatible with int, "no" given
bool(false)
bool(true)
TypeError: session_start(): Option "read_and_close" must be of type string|int|bool, float given
