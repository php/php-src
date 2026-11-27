--TEST--
Request hooks keep the session write at the place of the session module shutdown
--EXTENSIONS--
session
zend_test
--SKIPIF--
<?php
$extensions = array_map('strtolower', get_loaded_extensions());
if (array_search('session', $extensions) > array_search('zend_test', $extensions)) {
    die('skip session must be loaded before zend_test');
}
?>
--INI--
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
final class EchoHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    public function open(string $path, string $name): bool { return true; }
    public function close(): bool { return true; }
    public function read(string $id): string|false { return ''; }
    public function write(string $id, string $data): bool { echo "session write\n"; return true; }
    public function destroy(string $id): bool { return true; }
    public function gc(int $max_lifetime): int|false { return 0; }
    public function create_sid(): string { return 'order'; }
    public function validateId(string $id): bool { return true; }
    public function updateTimestamp(string $id, string $data): bool { return true; }
}

session_set_save_handler(new EchoHandler(), false);
session_id('order');
session_start();
$_SESSION['user'] = 'alice';
ini_set('zend_test.print_request_hooks', '1');
?>
--EXPECT--
request_end
request_shutdown
session write
