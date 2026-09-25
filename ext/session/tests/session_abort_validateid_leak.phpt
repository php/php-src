--TEST--
Session abort does not leak when validateId() returns wrong type
--INI--
session.use_strict_mode=1
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php
class MySession extends SessionHandler {
    public function validateId($key): bool {
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