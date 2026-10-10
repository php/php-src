--TEST--
IO hooks: pcntl_sigwaitinfo() and pcntl_sigtimedwait() are SigWait operations
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists("pcntl_sigwaitinfo")) die("skip pcntl_sigwaitinfo() is not available");
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

class Counting extends Scheduler
{
    public array $seen = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\SigWait) {
            $names = array_map(fn($s) => $s === SIGUSR1 ? 'USR1' : ($s === SIGUSR2 ? 'USR2' : $s), $op->getSignals());
            $this->seen[] = [$names, get_class($op->getHandle()), $op->getTimeout() !== null];
        }
        return parent::run($op);
    }
}

$scheduler = new Counting();
Io\Hooks\set_hooks($scheduler);

pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1, SIGUSR2]);

$scheduler->spawn(function () {
    $signo = pcntl_sigwaitinfo([SIGUSR1], $info);
    var_dump($signo === SIGUSR1, $info['signo'] === SIGUSR1, $info['pid'] === posix_getpid());
});

$scheduler->spawn(function () {
    $start = hrtime(true);
    var_dump(pcntl_sigtimedwait([SIGUSR2], $info, 0, 200000000));
    var_dump((hrtime(true) - $start) / 1e6 >= 150);
});

$scheduler->spawn(function () {
    usleep(50000);
    echo "sending\n";
    posix_kill(posix_getpid(), SIGUSR1);
});

$scheduler->loop();
var_dump($scheduler->seen);
?>
--EXPECT--
sending
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
array(2) {
  [0]=>
  array(3) {
    [0]=>
    array(1) {
      [0]=>
      string(4) "USR1"
    }
    [1]=>
    string(20) "Io\Poll\SignalHandle"
    [2]=>
    bool(false)
  }
  [1]=>
  array(3) {
    [0]=>
    array(1) {
      [0]=>
      string(4) "USR2"
    }
    [1]=>
    string(20) "Io\Poll\SignalHandle"
    [2]=>
    bool(true)
  }
}
