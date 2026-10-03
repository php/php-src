--TEST--
IO hooks: waiting for a child is an operation in proc_close(), pclose() and pcntl_waitpid()
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

class Counting extends Scheduler
{
    public array $seen = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\WaitPid) {
            $this->seen[] = ($op->getPid() > 0 ? "pid" : "any") . ":"
                . ($op->getHandle() === null ? "null" : get_class($op->getHandle()));
        }
        return parent::run($op);
    }
}

$scheduler = new Counting();
Io\Hooks\set_hooks($scheduler);
$results = [];
$start = hrtime(true);

// A forked child, waited for by pid; forked first, before anything is in
// flight. The child inherits the provider and a queue it must not use, so
// it drops the hooks before it blocks.
$scheduler->spawn(function () use (&$results) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        Io\Hooks\set_hooks(null);
        usleep(200000);
        exit(9);
    }
    $r = pcntl_waitpid($pid, $status);
    $results['fork'] = [$r === $pid, pcntl_wexitstatus($status)];
});

// proc_open() and proc_close()
$scheduler->spawn(function () use (&$results) {
    $proc = proc_open(['/bin/sh', '-c', 'sleep 0.3; exit 7'], [], $pipes);
    $results['proc'] = proc_close($proc);
});

// popen() and pclose()
$scheduler->spawn(function () use (&$results) {
    $fp = popen('sleep 0.1; echo out; exit 4', 'r');
    $results['popen'] = [fread($fp, 10), pclose($fp)];
});

$scheduler->loop();
ksort($results);
var_dump($results);
// The three waits overlapped instead of adding up
var_dump((hrtime(true) - $start) / 1e9 < 0.55);
var_dump($scheduler->seen);

// Any child, with WNOHANG, never reaches the provider
$before = count($scheduler->seen);
var_dump(pcntl_waitpid(-1, $status, WNOHANG));
var_dump(count($scheduler->seen) === $before);
?>
--EXPECT--
array(3) {
  ["fork"]=>
  array(2) {
    [0]=>
    bool(true)
    [1]=>
    int(9)
  }
  ["popen"]=>
  array(2) {
    [0]=>
    string(4) "out
"
    [1]=>
    int(4)
  }
  ["proc"]=>
  int(7)
}
bool(true)
array(3) {
  [0]=>
  string(25) "pid:Io\Poll\ProcessHandle"
  [1]=>
  string(25) "pid:Io\Poll\ProcessHandle"
  [2]=>
  string(25) "pid:Io\Poll\ProcessHandle"
}
int(-1)
bool(true)
