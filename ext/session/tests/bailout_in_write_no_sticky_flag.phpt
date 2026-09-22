--TEST--
Bailout in userland save handler write() must not leave in_save_handler sticky (GH-15529)
--INI--
memory_limit=16M
display_errors=1
--FILE--
<?php
class Handler implements SessionHandlerInterface
{
    public static int $writes = 0;
    public function open($path, $name): bool { return true; }
    public function close(): bool { echo "close\n"; return true; }
    public function read($id): string|false { return ''; }
    public function write($id, $data): bool {
        self::$writes++;
        if (self::$writes === 1) {
            /* Force a real bailout (fatal OOM), not an unwind-exit */
            $GLOBALS['oom'] = str_repeat('a', 64 * 1024 * 1024);
        }
        echo "write ", self::$writes, "\n";
        return true;
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
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
write 2
close
