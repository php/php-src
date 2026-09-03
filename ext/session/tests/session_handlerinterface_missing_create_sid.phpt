--TEST--
SessionHandlerInterface missing create_sid() emits deprecation
--FILE--
<?php

class MySessionHandler implements SessionHandlerInterface
{
    public function open(string $path, string $name): bool
    {
        return true;
    }

    public function close(): bool
    {
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

?>
--EXPECTF--
Deprecated: Class MySessionHandler implementing SessionHandlerInterface is missing the create_sid() method which will be required in PHP 9.0 in %s on line %d

Deprecated: Class MySessionHandler implementing SessionHandlerInterface is missing the validateId() method which will be required in PHP 9.0 in %s on line %d
