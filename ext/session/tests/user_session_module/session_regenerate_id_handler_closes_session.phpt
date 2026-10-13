--TEST--
session_regenerate_id() when the close handler destroys the session
--INI--
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
Warning: Class MySessionHandler implementing SessionHandlerInterface is missing the create_sid() method which will be required in PHP 9.0 in %s on line %d

Warning: Class MySessionHandler implementing SessionHandlerInterface is missing the validateId() method which will be required in PHP 9.0 in %s on line %d

Deprecated: session_set_save_handler(): Providing an object to argument #1 ($sessionhandler) which does not have the create_sid() method defined is deprecated in %s on line %d

Deprecated: session_set_save_handler(): Providing an object to argument #1 ($sessionhandler) which does not have the validateId() method defined is deprecated in %s on line %d

Warning: session_destroy(): Cannot call session save handler in a recursive manner in %s on line %d

Warning: session_destroy(): Session object destruction failed in %s on line %d

Warning: session_regenerate_id(): Session ID cannot be regenerated because the save handler closed the session in %s on line %d
bool(false)
bool(true)
