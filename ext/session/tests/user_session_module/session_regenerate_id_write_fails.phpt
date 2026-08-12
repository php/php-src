--TEST--
session_regenerate_id(false): warns and returns false when save handler's write() fails
--EXTENSIONS--
session
--INI--
session.save_path=
--FILE--
<?php

class FailingWriteHandler implements SessionHandlerInterface {
    function open($save_path, $session_name): bool {
        return true;
    }

    function close(): bool {
        return true;
    }

    function read($id): string|false {
        return '';
    }

    function write($id, $session_data): bool {
        return false;
    }

    function destroy($id): bool {
        return true;
    }

    function gc($maxlifetime): int|false {
        return 0;
    }
}

session_set_save_handler(new FailingWriteHandler());
session_start();
$_SESSION['foo'] = 'bar';
$old_id = session_id();

var_dump(session_regenerate_id(false));
var_dump(session_id() === $old_id);
var_dump(session_status() === PHP_SESSION_NONE);

?>
--EXPECTF--
Warning: session_regenerate_id(): Session write failed. ID: user (path: ) in %s on line %d
bool(false)
bool(true)
bool(true)
