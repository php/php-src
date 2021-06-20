--TEST--
Bug #69111 Crash in SessionHandler::read()
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$sh = new SessionHandler;
session_set_save_handler($sh);

$savePath = ini_get('session.save_path');
$sessionName = ini_get('session.name');

// session_start(); // Uncommenting this makes it not crash when reading the session (see below), but it will not return any data.

try {
    $sh->open($savePath, $sessionName);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $sh->write("foo", "bar");
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $sh->read("");
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Session is not active
Session is not active
Session is not active
