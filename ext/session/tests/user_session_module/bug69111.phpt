--TEST--
Bug #69111 Crash in SessionHandler::read()
--EXTENSIONS--
session
--FILE--
<?php
$sh = new SessionHandler;
session_set_save_handler($sh);

// session_start(); // Uncommenting this makes it not crash when reading the session (see below), but it will not return any data.

try {
    $sh->open('path', 'name');
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
