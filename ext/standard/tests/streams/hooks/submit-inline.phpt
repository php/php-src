--TEST--
IO hooks: submit() returns an operation the queue completed at submit, and the provider does not suspend
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip no unix datagram sockets');
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $log = [];
    private \Io\OperationQueue $q;

    public function __construct()
    {
        $this->q = self::defaultQueue();
        parent::__construct($this->q, [Io\Hooks\Capability::EdgeRegistrations]);
    }

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Timer) {
            return parent::run($op);
        }
        $c = $this->q->submit($op, \Fiber::getCurrent());
        if ($c !== null) {
            $this->log[] = substr($op::class, 13) . ' at submit ' . $c->getStatus()->name;
            return $c;
        }
        $this->log[] = substr($op::class, 13) . ' waited';
        $this->inFlight++;
        return \Fiber::suspend();
    }
}

// Datagram waits are readiness ops after EAGAIN on both queues
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_DGRAM, 0);
$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($a) {
    // The first wait is real. A datagram arriving while nothing waits sets the pair's ready bit
    // and the syscall takes it without an op, so the wait after that is answered at submit, once,
    // and the next waits for real.
    var_dump(stream_socket_recvfrom($a, 10));
    usleep(30000);
    var_dump(stream_socket_recvfrom($a, 10));
    var_dump(stream_socket_recvfrom($a, 10));
});
$scheduler->spawn(function () use ($b) {
    usleep(10000);
    fwrite($b, "one");
    usleep(10000);
    fwrite($b, "two");
    usleep(40000);
    fwrite($b, "three");
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
print_r($scheduler->log);
?>
--EXPECT--
string(3) "one"
string(3) "two"
string(5) "three"
Array
(
    [0] => Poll waited
    [1] => Poll at submit Done
    [2] => Poll waited
)
