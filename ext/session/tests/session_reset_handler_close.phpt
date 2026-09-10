--TEST--
Session reset closes the save handler before reopening it
--EXTENSIONS--
session
--INI--
session.use_cookies=0
session.gc_probability=0
--FILE--
<?php

class LoggingHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    public function open(string $path, string $name): bool
    {
        $GLOBALS['calls'][] = 'open';
        return true;
    }
    public function close(): bool
    {
        $GLOBALS['calls'][] = 'close';
        return true;
    }
    public function read(string $id): string|false
    {
        $GLOBALS['calls'][] = 'read';
        return '';
    }
    public function write(string $id, string $data): bool
    {
        $GLOBALS['calls'][] = 'write';
        return true;
    }
    public function destroy(string $id): bool
    {
        $GLOBALS['calls'][] = 'destroy';
        return true;
    }
    public function gc(int $max_lifetime): int|false
    {
        $GLOBALS['calls'][] = 'gc';
        return 0;
    }
    public function create_sid(): string
    {
        return 'testsessionid';
    }
    public function validateId(string $id): bool
    {
        return true;
    }
    public function updateTimestamp(string $id, string $data): bool
    {
        return true;
    }
}

$GLOBALS['calls'] = [];
session_set_save_handler(new LoggingHandler(), true);
var_dump(session_start());
var_dump(session_reset());
print_r($GLOBALS['calls']);

?>
--EXPECT--
bool(true)
bool(true)
Array
(
    [0] => open
    [1] => read
    [2] => close
    [3] => open
    [4] => read
)
