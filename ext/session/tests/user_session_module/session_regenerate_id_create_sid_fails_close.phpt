--TEST--
session_regenerate_id() closes the save handler when create_sid fails
--INI--
session.use_cookies=0
session.gc_probability=0
session.save_path=
--EXTENSIONS--
session
--FILE--
<?php

class CountingHandler extends SessionHandler
{
    public int $opens = 0;
    public int $closes = 0;
    public int $createCalls = 0;

    public function open($path, $name): bool
    {
        $this->opens++;
        return true;
    }

    public function close(): bool
    {
        $this->closes++;
        return true;
    }

    public function read($id): string
    {
        return '';
    }

    public function write($id, $data): bool
    {
        return true;
    }

    public function destroy($id): bool
    {
        return true;
    }

    public function gc($maxlifetime): int|false
    {
        return 0;
    }

    public function create_sid(): string
    {
        if (++$this->createCalls === 2) {
            throw new RuntimeException('create_sid failed');
        }
        return parent::create_sid();
    }
}

$h = new CountingHandler();
session_set_save_handler($h, true);
$started = session_start();
try {
    session_regenerate_id();
} catch (Throwable $e) {
    $msg = $e::class . ': ' . $e->getMessage();
}
session_module_name('files');
echo $msg ?? 'no exception', PHP_EOL;
var_dump($started);
var_dump(session_status() === PHP_SESSION_NONE);
echo "open={$h->opens} close={$h->closes}\n";

?>
--EXPECT--
Error: Session id must be a string
bool(true)
bool(true)
open=2 close=1
