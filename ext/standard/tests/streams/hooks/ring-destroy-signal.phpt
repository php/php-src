--TEST--
Io\Ring\Engine: a signal while the ring is drained on destruction
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
pcntl_signal(SIGUSR1, function () { echo "SIGUSR1 handled\n"; });

$ring = new Io\Ring\Engine();
Io\Hooks\set_hooks(new class($ring) implements Io\Hooks\Hooks {
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        $this->ring->submit($op);
        throw new RuntimeException("gave up");
    }
});

$parent = getmypid();
$pid = pcntl_fork();
if ($pid === 0) {
    Io\Hooks\set_hooks(null);
    usleep(200000);
    posix_kill($parent, SIGUSR1);
    usleep(200000);
    exit(3);
}

// A wait for any child blocks a worker and cannot be cancelled: the
// destruction waits for it and is interrupted by the signal meanwhile
try {
    pcntl_waitpid(-1, $status);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);
unset($ring);
echo "ring destroyed\n";
var_dump(pcntl_waitpid($pid, $status) === $pid, pcntl_wexitstatus($status));
pcntl_signal_dispatch();
?>
--EXPECT--
gave up
ring destroyed
bool(true)
int(3)
SIGUSR1 handled
