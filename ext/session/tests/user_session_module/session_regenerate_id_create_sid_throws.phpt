--TEST--
session_regenerate_id() preserves exceptions from create_sid(), including collision retries
--EXTENSIONS--
session
--INI--
session.use_cookies=0
session.cache_limiter=
session.use_strict_mode=1
session.gc_probability=0
--FILE--
<?php

ob_start();

class FailingHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    public int $calls = 0;
    public int $throwAt;

    public function open($path, $name): bool { return true; }
    public function close(): bool { return true; }
    public function read($id): string|false { return ''; }
    public function write($id, $data): bool { return true; }
    public function destroy($id): bool { return true; }
    public function gc($max_lifetime): int|false { return 0; }
    public function updateTimestamp($id, $data): bool { return true; }
    public function validateId($id): bool { return true; }

    public function create_sid(): string
    {
        if (++$this->calls === $this->throwAt) {
            throw new RuntimeException('create_sid failed');
        }
        return 'session' . $this->calls;
    }
}

foreach ([2, 3] as $throwAt) {
    $handler = new FailingHandler();
    $handler->throwAt = $throwAt;
    session_set_save_handler($handler);
    session_id('');
    session_start();

    try {
        session_regenerate_id();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    echo 'create_sid calls: ', $handler->calls, PHP_EOL;
}
?>
--EXPECT--
RuntimeException: create_sid failed
create_sid calls: 2
RuntimeException: create_sid failed
create_sid calls: 3
