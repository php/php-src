--TEST--
IO hooks: WaitPid and SigWait through their handles on a provider's own Poll context
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsProcessHandles()) die("skip no process handle source");
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source");
if (!function_exists("pcntl_sigwaitinfo")) die("skip pcntl_sigwaitinfo() is not available");
?>
--FILE--
<?php
include __DIR__ . '/context-provider.inc';

/* Records what the operations expose to a provider that watches handles */
final class Tracing extends ContextProvider
{
    public array $seen = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\WaitPid || $op instanceof \Io\Operation\SigWait) {
            $events = array_map(fn ($e) => $e->name, $op->getEvents());
            $this->seen[] = substr(get_class($op), 13) . ':' . get_class($op->getHandle()) . ':' . implode(',', $events);
        }
        return parent::run($op);
    }
}

$provider = new Tracing();
\Io\Hooks\set_hooks($provider);
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1, SIGUSR2]);

$provider->spawn(function () {
    // The context observes the exit through the ProcessHandle; proc_close()
    // collects the child after Ready. It exits well after the timed wait
    // below runs out, so the output order is fixed.
    $proc = proc_open(['/bin/sh', '-c', 'sleep 0.5; exit 6'], [], $pipes);
    var_dump(proc_close($proc));
});
$provider->spawn(function () {
    // The context consumes the signal through the SignalHandle; the wait
    // gets the recorded info
    $signo = pcntl_sigwaitinfo([SIGUSR1], $info);
    var_dump($signo === SIGUSR1, $info['signo'] === SIGUSR1, $info['pid'] === posix_getpid());
});
$provider->spawn(function () {
    // A timed wait that runs out is the timer pair's Timeout
    var_dump(pcntl_sigtimedwait([SIGUSR2], $info, 0, 100000000));
});
$provider->spawn(function () {
    usleep(30000);
    posix_kill(posix_getpid(), SIGUSR1);
    echo "sent\n";
});

$provider->loop();
var_dump($provider->seen);
?>
--EXPECT--
sent
bool(true)
bool(true)
bool(true)
bool(false)
int(6)
array(3) {
  [0]=>
  string(37) "WaitPid:Io\Poll\ProcessHandle:Process"
  [1]=>
  string(35) "SigWait:Io\Poll\SignalHandle:Signal"
  [2]=>
  string(35) "SigWait:Io\Poll\SignalHandle:Signal"
}
