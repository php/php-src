--TEST--
SessionHandler::validateId must return bool
--INI--
session.use_strict_mode=1
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MySession extends SessionHandler {
    public function validateId($key) {
        return null;
    }
}

$handler = new MySession();

try {
    session_set_save_handler($handler);
    session_start();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

session_write_close();

try {
    session_start();
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Session id must be a string
