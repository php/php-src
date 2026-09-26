--TEST--
Exceptions from destruction of an invalid create_sid() return value are preserved
--EXTENSIONS--
session
--FILE--
<?php

class FailingHandler implements SessionHandlerInterface, SessionIdInterface
{
    public function open($path, $name): bool { return true; }
    public function close(): bool { return true; }
    public function read($id): string|false { return ''; }
    public function write($id, $data): bool { return true; }
    public function destroy($id): bool { return true; }
    public function gc($max_lifetime): int|false { return 0; }
    public function validateId($id): bool { return true; }

    #[ReturnTypeWillChange]
    public function create_sid()
    {
        return new class {
            public function __destruct()
            {
                throw new RuntimeException('destructor failed');
            }
        };
    }
}

session_set_save_handler(new FailingHandler());
try {
    session_start();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
RuntimeException: destructor failed
