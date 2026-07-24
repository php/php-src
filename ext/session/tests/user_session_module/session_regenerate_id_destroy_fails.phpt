--TEST--
session_regenerate_id(true): warns and returns false when save handler's destroy() fails
--EXTENSIONS--
session
--INI--
session.save_path=
--FILE--
<?php

class FailingDestroyHandler implements SessionHandlerInterface {
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
        return true;
    }

    function destroy($id): bool {
        return false;
    }

    function gc($maxlifetime): int|false {
        return 0;
    }
}

session_set_save_handler(new FailingDestroyHandler());
session_start();
$_SESSION['foo'] = 'bar';
$old_id = session_id();

var_dump(session_regenerate_id(true));
var_dump(session_id() === $old_id);
var_dump(session_status() === PHP_SESSION_NONE);

?>
--EXPECTF--
Warning: session_regenerate_id(): Session object destruction failed. ID: user (path: ) in %s on line %d
bool(false)
bool(true)
bool(true)
