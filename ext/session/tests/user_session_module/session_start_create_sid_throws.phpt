--TEST--
session_start() preserves exceptions from create_sid()
--EXTENSIONS--
session
--INI--
session.use_cookies=0
session.cache_limiter=
session.gc_probability=0
--FILE--
<?php

ob_start();

class FailingHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    public bool $invalidReturn = false;

    public function open($path, $name): bool { return true; }
    public function close(): bool { return true; }
    public function read($id): string|false { return ''; }
    public function write($id, $data): bool { return true; }
    public function destroy($id): bool { return true; }
    public function gc($max_lifetime): int|false { return 0; }
    public function updateTimestamp($id, $data): bool { return true; }

    public function create_sid(): string
    {
        if ($this->invalidReturn) {
            return [];
        }
        throw new RuntimeException('create_sid failed');
    }

    public function validateId(string $id): bool
    {
        return false;
    }
}

$handler = new FailingHandler();
session_set_save_handler($handler);

foreach ([false, true] as $strict) {
    foreach ([false, true] as $invalidReturn) {
        echo 'strict mode: ', (int) $strict, ', invalid return: ', (int) $invalidReturn, PHP_EOL;
        $handler->invalidReturn = $invalidReturn;
        session_id($strict ? 'rejected' : '');
        try {
            session_start(['use_strict_mode' => $strict]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
}
?>
--EXPECT--
strict mode: 0, invalid return: 0
RuntimeException: create_sid failed
strict mode: 0, invalid return: 1
TypeError: FailingHandler::create_sid(): Return value must be of type string, array returned
strict mode: 1, invalid return: 0
RuntimeException: create_sid failed
strict mode: 1, invalid return: 1
TypeError: FailingHandler::create_sid(): Return value must be of type string, array returned
