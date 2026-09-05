--TEST--
session_regenerate_id() when the close handler destroys the session
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

class MySessionHandler implements SessionHandlerInterface
{
    private bool $destroyed = false;

    public function open(string $path, string $name): bool
    {
        return true;
    }

    public function close(): bool
    {
        if (!$this->destroyed) {
            $this->destroyed = true;
            session_destroy();
        }
        return true;
    }

    public function read(string $id): string|false
    {
        return '';
    }

    public function write(string $id, string $data): bool
    {
        return true;
    }

    public function destroy(string $id): bool
    {
        return true;
    }

    public function gc(int $max_lifetime): int|false
    {
        return 0;
    }
}

session_set_save_handler(new MySessionHandler(), true);
session_start();

var_dump(session_regenerate_id(false));
var_dump(session_status() === PHP_SESSION_NONE);

?>
--EXPECTF--
Warning: session_destroy(): Cannot call session save handler in a recursive manner in %s on line %d

Warning: session_destroy(): Session object destruction failed in %s on line %d

Warning: session_regenerate_id(): Session ID cannot be regenerated because the save handler closed the session in %s on line %d
bool(false)
bool(true)
