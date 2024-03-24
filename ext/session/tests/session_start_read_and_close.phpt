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
    session_start(["read_and_close" => $value]);
    var_dump(session_status() === PHP_SESSION_ACTIVE);
    session_write_close();
}

try {
    session_start(["read_and_close" => 1.1]);
} catch (Exception $e) {
    echo $e->getMessage();
}

ob_end_flush();
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Fatal error: Uncaught TypeError: session_start(): Option "read_and_close" must be of type string|int|bool, float given in %s:%d
Stack trace:
#0 %s(%d): session_start(Array)
#1 {main}
  thrown in %s on line %d
