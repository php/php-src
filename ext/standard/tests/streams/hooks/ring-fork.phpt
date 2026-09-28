--TEST--
Io\Ring\Engine: a forked child cannot use the parent's ring and does not keep its descriptors
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (!is_dir('/proc/self/fd')) die("skip needs /proc/self/fd");
?>
--FILE--
<?php
function fds(): array {
    // Not /proc/self: the realpath cache would resolve it to the parent in the child
    $fds = array_values(array_diff(scandir('/proc/' . posix_getpid() . '/fd'), ['.', '..']));
    sort($fds);
    return $fds;
}

final class RingProvider implements Io\Hooks\Hooks
{
    public bool $giveUp = false;
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op);
        if ($this->giveUp) {
            throw new RuntimeException("gave up");
        }
        do {
            $completions = $this->ring->waitCompletions();
        } while (!$completions);
        return $completions[0];
    }
}

pcntl_sigprocmask(SIG_BLOCK, [SIGUSR2]);
$pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$before = fds();
$ring = new Io\Ring\Engine();
$handle = $ring->getHandle();
$provider = new RingProvider($ring);
Io\Hooks\set_hooks($provider);
usleep(1000);
// Left in flight: the child inherits a record whose completion it never sees
$provider->giveUp = true;
try {
    usleep(200000);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
// What ior opens on demand: the thread backend's poller, io_uring's signalfd
foreach ([fn() => fread($pair[0], 1), fn() => pcntl_sigwaitinfo([SIGUSR2])] as $call) {
    try {
        @$call();
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
}
$provider->giveUp = false;
Io\Hooks\set_hooks(null);

$pid = pcntl_fork();
if ($pid === 0) {
    var_dump(fds() === $before);
    $calls = [
        'getBackend' => fn() => $ring->getBackend(),
        'getHandle' => fn() => $ring->getHandle(),
        'getHookCapabilities' => fn() => $ring->getHookCapabilities(),
        'countPending' => fn() => $ring->countPending(),
        'waitCompletions' => fn() => $ring->waitCompletions(Time\Duration::fromSeconds(0)),
    ];
    foreach ($calls as $name => $call) {
        try {
            $call();
            echo "$name: no exception\n";
        } catch (Io\Ring\RingException $e) {
            echo "$name: ", $e->getMessage(), "\n";
        }
    }
    // Neither submits nor waits for the parent's operations
    unset($calls, $ring, $provider);
    var_dump(fds() === $before);
    exit(3);
}

var_dump(pcntl_waitpid($pid, $status) === $pid, pcntl_wexitstatus($status));

// The parent's ring is unaffected
var_dump($ring->waitCompletions(), $ring->countPending());
Io\Hooks\set_hooks($provider);
$start = hrtime(true);
usleep(20000);
var_dump((hrtime(true) - $start) / 1e6 >= 15);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
gave up
gave up
gave up
bool(true)
getBackend: The ring was created in another process
getHandle: The ring was created in another process
getHookCapabilities: The ring was created in another process
countPending: The ring was created in another process
waitCompletions: The ring was created in another process
bool(true)
bool(true)
int(3)
array(0) {
}
int(0)
bool(true)
