--TEST--
GH-23061 (SessionHandler::create_sid() callback failure leaks memory in debug build)
--EXTENSIONS--
session
--FILE--
<?php
class a extends SessionHandler {
    function create_sid(): string {
        return [3, 6];
    }
}
$c = new a;
session_set_save_handler($c);
try {
    session_start();
} catch (Error $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Session id must be a string
