--TEST--
GH-23043 (broken session id code can cause zend_mm_heap corrupted)
--EXTENSIONS--
session
--CREDITS--
lmaltsis
--FILE--
<?php
ob_start();
class a extends SessionHandler {
    function read($b): string {
        return "";
    }
    function create_sid(): string {
        var_dump(session_id());
        return '';
    }
}
$c = new a;
session_set_save_handler($c);
session_start();
session_write_close();
session_start();
?>
--EXPECTF--
string(0) ""

Warning: SessionHandler::write(): Session ID is too long or contains illegal characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d

Warning: session_write_close(): Failed to write session data using user defined save handler. (session.save_path: , handler: a::write) in %s on line %d
string(0) ""

Warning: SessionHandler::write(): Session ID is too long or contains illegal characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in Unknown on line 0

Warning: session_write_close(): Failed to write session data using user defined save handler. (session.save_path: , handler: a::write) in Unknown on line 0
