--TEST--
GH-22616: Session GC randomness is independent after a fork
--EXTENSIONS--
session
pcntl
--INI--
session.gc_probability=1
session.gc_divisor=2
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
$handler = new class implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface {
    public bool $collected = false;

    public function create_sid(): string { return 'gh22616'; }
    public function validateId(string $id): bool { return true; }
    public function updateTimestamp(string $id, string $data): bool { return true; }
    public function open(string $path, string $name): bool { return true; }
    public function close(): bool { return true; }
    public function read(string $id): string|false { return ''; }
    public function write(string $id, string $data): bool { return true; }
    public function destroy(string $id): bool { return true; }
    public function gc(int $max_lifetime): int|false {
        $this->collected = true;
        return 0;
    }
};
session_set_save_handler($handler);
session_id('gh22616');

function sequence(): string {
    global $handler;
    if (session_status() === PHP_SESSION_ACTIVE) {
        session_write_close();
    }
    $result = '';
    for ($i = 0; $i < 256; $i++) {
        $handler->collected = false;
        session_start();
        session_write_close();
        $result .= (int) $handler->collected;
    }
    return $result;
}

$results = [];
foreach (['unseeded', 'seeded', 'active'] as $state) {
    if ($state === 'active') {
        session_start();
    }
    $file = tempnam(sys_get_temp_dir(), 'gh22616');
    $pid = pcntl_fork();
    if ($pid === -1) {
        throw new Exception('fork failed');
    }
    if ($pid === 0) {
        file_put_contents($file, sequence());
        exit;
    }
    $parent = sequence();
    pcntl_waitpid($pid, $status);
    $child = file_get_contents($file);
    unlink($file);
    $results[$state] = pcntl_wifexited($status)
        && pcntl_wexitstatus($status) === 0
        && strlen($child) === 256
        && $parent !== $child;
}
var_dump($results);
?>
--EXPECT--
array(3) {
  ["unseeded"]=>
  bool(true)
  ["seeded"]=>
  bool(true)
  ["active"]=>
  bool(true)
}
