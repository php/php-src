--TEST--
Io\Ring\Engine: operations without a stream outlive their frame on the ring's own buffers
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
$ring = new Io\Ring\Engine();
Io\Hooks\set_hooks(new class($ring) implements Io\Hooks\Hooks {
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        // Submitted, then the frame goes away with the operation still in flight
        $this->ring->submit($op);
        throw new RuntimeException("gave up on " . $op::class);
    }
});

$pid = pcntl_fork();
if ($pid === 0) {
    Io\Hooks\set_hooks(null);
    usleep(100000);
    exit(7);
}

try {
    pcntl_waitpid(-1, $status);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
try {
    gethostbynamel("localhost");
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);

function deep($n) { $a = str_repeat('x', 100); return $n ? deep($n - 1) . '' : $a; }
deep(50);

// Only orphans are left: they are counted, and the wait ends once they settled
var_dump($ring->countPending());
var_dump($ring->waitCompletions());
var_dump($ring->countPending());

// Where the ring reaped the child, its status went to the next wait for it
var_dump(pcntl_waitpid($pid, $status) === $pid, pcntl_wexitstatus($status));
?>
--EXPECT--
gave up on Io\Operation\WaitPid
gave up on Io\Operation\GetAddrInfo
int(2)
array(0) {
}
int(0)
bool(true)
int(7)
