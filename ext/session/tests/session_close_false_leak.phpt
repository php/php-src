--TEST--
Session close handler returning false does not leak memory
--INI--
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php
class MySession extends SessionHandler {
    public function close(): bool {
        return false;
    }
}

$handler = new MySession();
session_set_save_handler($handler);
session_start();
session_write_close();
?>
--EXPECT--