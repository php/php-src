--TEST--
IO hooks: proc_close() waits with a WaitPid operation, so waits for several children overlap
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip no proc_open');
if (PHP_OS_FAMILY === 'Windows' && getenv('IO_HOOKS_QUEUE') !== 'ring') {
    die('skip the poll queue has no process source on Windows: the core waits synchronously');
}
?>
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

foreach ([7, 8] as $code) {
    $scheduler->spawn(function () use (&$results, $code) {
        $proc = proc_open([PHP_BINARY, '-n', '-r', "usleep(300000); exit($code);"], [], $pipes);
        $results[$code] = proc_close($proc);
    });
}
$scheduler->loop();
ksort($results);
var_dump($results);
// The two waits overlapped instead of adding up
var_dump((hrtime(true) - $start) / 1e9 < 0.55);
var_dump($scheduler->seen);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
array(2) {
  [7]=>
  int(7)
  [8]=>
  int(8)
}
bool(true)
array(2) {
  [0]=>
  string(25) "pid:Io\Poll\ProcessHandle"
  [1]=>
  string(25) "pid:Io\Poll\ProcessHandle"
}
