--TEST--
Io\Ring\Engine: a signal taken by an abandoned SigWait goes back to the process
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1]);

$ring = new Io\Ring\Engine();
Io\Hooks\set_hooks(new class($ring) implements Io\Hooks\Hooks {
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getHookCapabilities(); }
    public function add(Io\Operation $op): void {}
    public function remove(Io\Operation $op): void {}
    public function run(Io\Operation $op): Io\Completion {
        $this->ring->submit($op);
        // The signal arrives and the ring takes it, then the caller leaves without the completion
        posix_kill(getmypid(), SIGUSR1);
        $until = hrtime(true) + 100_000_000;
        while (hrtime(true) < $until);
        throw new RuntimeException("gave up on " . $op::class);
    }
});

try {
    @pcntl_sigwaitinfo([SIGUSR1], $info);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);
var_dump($ring->waitCompletions(), $ring->countPending());

var_dump(pcntl_sigtimedwait([SIGUSR1], $info, 1) === SIGUSR1, $info['pid'] === getmypid());
?>
--EXPECT--
gave up on Io\Operation\SigWait
array(0) {
}
int(0)
bool(true)
bool(true)
