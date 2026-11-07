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
    $file1 = tempnam(sys_get_temp_dir(), 'gh22616');
    $pid1 = pcntl_fork();
    if ($pid1 === -1) {
        throw new Exception('fork failed');
    }
    if ($pid1 === 0) {
        file_put_contents($file1, sequence());
        exit;
    }

    $file2 = tempnam(sys_get_temp_dir(), 'gh22616');
    $pid2 = pcntl_fork();
    if ($pid2 === -1) {
        throw new Exception('fork failed');
    }
    if ($pid2 === 0) {
        file_put_contents($file2, sequence());
        exit;
    }

    $parent = sequence();
    pcntl_waitpid($pid1, $status1);
    pcntl_waitpid($pid2, $status2);
    $child1 = file_get_contents($file1);
    $child2 = file_get_contents($file2);
    unlink($file1);
    unlink($file2);
    $results[$state] = pcntl_wifexited($status1)
        && pcntl_wexitstatus($status1) === 0
        && pcntl_wifexited($status2)
        && pcntl_wexitstatus($status2) === 0
        && strlen($child1) === 256
        && strlen($child2) === 256
        && $parent !== $child1
        && $parent !== $child2
        && $child1 !== $child2;
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
