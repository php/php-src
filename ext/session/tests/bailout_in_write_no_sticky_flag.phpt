--TEST--
GH-15529: Bailout in userland save handler write() is not retried, close() still runs
--EXTENSIONS--
session
zend_test
--FILE--
<?php
class Handler implements SessionHandlerInterface, SessionIdInterface
{
    public function create_sid(): string { return 'gh15529'; }
    public function validateId(string $id): bool { return true; }
    public function open($path, $name): bool { return true; }
    public function close(): bool { echo "close\n"; return true; }
    public function read($id): string|false { return ''; }
    public function write($id, $data): bool {
        echo "write\n";
        zend_trigger_bailout();
    }
    public function destroy($id): bool { return true; }
    public function gc(int $max_lifetime): int|false { return 0; }
}
session_set_save_handler(new Handler(), true);
session_start();
$_SESSION['x'] = 1;
session_write_close();
echo "not reached\n";
?>
--EXPECTF--
write

Fatal error: Bailout in %s on line %d
close
