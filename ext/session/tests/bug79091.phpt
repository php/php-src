--TEST--
Bug #79091 (heap use-after-free in session_create_id())
--EXTENSIONS--
session
--FILE--
<?php
class MySessionHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    public function close(): bool
    {
        return true;
    }

    public function destroy($session_id): bool
    {
        return true;
    }

    public function gc($maxlifetime): int|false
    {
        return true;
    }

    public function open($save_path, $session_name): bool
    {
        return true;
    }

    public function read($session_id): string|false
    {
        return '';
    }

    public function write($session_id, $session_data): bool
    {
        return true;
    }

    public function create_sid(): string
    {
        return uniqid();
    }

    public function updateTimestamp($key, $val): bool
    {
        return true;
    }

    public function validateId($key): bool
    {
        return true;
    }
}

ob_start();
var_dump(session_set_save_handler(new MySessionHandler()));
var_dump(session_start());
ob_flush();
session_create_id();
?>
--EXPECTF--
bool(true)
bool(true)

Warning: session_create_id(): Failed to create new ID in %s on line %d
