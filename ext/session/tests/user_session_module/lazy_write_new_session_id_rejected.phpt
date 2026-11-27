--TEST--
lazy_write stores a new empty session when strict mode rejects the ID
--EXTENSIONS--
session
--INI--
session.use_strict_mode=1
session.lazy_write=1
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
class Handler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    private array $storage = [];

    public function open($path, $name): bool { return true; }
    public function close(): bool { return true; }
    public function read($id): string|false { return $this->storage[$id] ?? ''; }
    public function write($id, $data): bool { echo "write\n"; $this->storage[$id] = $data; return true; }
    public function destroy($id): bool { return true; }
    public function gc($max_lifetime): int|false { return 0; }
    public function create_sid(): string { return bin2hex(random_bytes(16)); }
    public function validateId($id): bool { return isset($this->storage[$id]); }
    public function updateTimestamp($id, $data): bool { echo "updateTimestamp\n"; return true; }
}

session_set_save_handler(new Handler());

session_id('unknown');
session_start();
var_dump(session_id() !== 'unknown');
$id = session_id();
session_commit();

session_id($id);
session_start();
var_dump(session_id() === $id);
session_commit();
?>
--EXPECT--
bool(true)
write
bool(true)
updateTimestamp
