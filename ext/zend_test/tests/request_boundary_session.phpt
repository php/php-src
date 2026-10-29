--TEST--
Request boundary writes and closes the session and keeps the save handler
--EXTENSIONS--
session
zend_test
--INI--
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
--FILE--
<?php
class MemoryHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface {
    public static array $data = [];
    public function open(string $path, string $name): bool { return true; }
    public function close(): bool { echo "close\n"; return true; }
    public function read(string $id): string|false { return self::$data[$id] ?? ''; }
    public function write(string $id, string $data): bool { echo "write $id\n"; self::$data[$id] = $data; return true; }
    public function destroy(string $id): bool { return true; }
    public function gc(int $max_lifetime): int|false { return 0; }
    public function create_sid(): string { return 'generated'; }
    public function validateId(string $id): bool { return true; }
    public function updateTimestamp(string $id, string $data): bool { return true; }
}

session_set_save_handler(new MemoryHandler());
session_id('previous');
session_start();
$_SESSION['user'] = 'alice';

var_dump(zend_test_request_boundary());

var_dump(session_status() === PHP_SESSION_NONE, session_id(), isset($_SESSION));

session_id('next');
session_start();
var_dump($_SESSION);
session_write_close();

session_id('previous');
session_start();
var_dump($_SESSION);
session_write_close();
?>
--EXPECT--
write previous
close
bool(true)
bool(true)
string(0) ""
bool(false)
array(0) {
}
close
array(1) {
  ["user"]=>
  string(5) "alice"
}
close
