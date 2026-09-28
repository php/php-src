--TEST--
IO hooks: a Socket sharing a stream's descriptor operates on the stream, with the stream's handle
--EXTENSIONS--
sockets
--SKIPIF--
<?php
/* The registrations of the pairs are the point: the ring offers none on IOCP */
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';
$queue = Scheduler::defaultQueue();
$caps = $queue instanceof Io\Ring\Engine ? $queue->getSupportedHookCapabilities() : $queue->getHookCapabilities();
if (!in_array(Io\Hooks\Capability::EdgeRegistrations, $caps, true) && !in_array(Io\Hooks\Capability::LevelRegistrations, $caps, true)) {
    die('skip the queue takes no registrations');
}
?>
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

final class Tracing extends Scheduler
{
    public ?StreamPollWeakHandle $target = null;
    public array $ops = [];
    public array $adds = [];

    /* What the imported Socket's stream sees; the writer's own ops are left out */
    public function run(\Io\Operation $op): \Io\Completion
    {
        if (!$op instanceof \Io\Operation\Timer && !$op instanceof \Io\Operation\Any
                && $op->getHandle() === $this->target) {
            $reg = $op->getRegistration();
            $this->ops[] = substr($op::class, 13) . ($reg ? ' reg ' . get_class($reg->getHandle()) : '');
        }
        return parent::run($op);
    }

    public function add(\Io\Registration $r): void
    {
        if ($r->getHandle() === $this->target) {
            $this->adds[] = $r->getEvent()->name . ' ' . get_class($r->getHandle());
        }
        parent::add($r);
    }
}

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$s = socket_import_stream($a);
// Registrations on both queues, direct ops where the queue performs them
$scheduler = new Tracing(null, [Io\Hooks\Capability::EdgeRegistrations, Io\Hooks\Capability::LevelRegistrations,
    Io\Hooks\Capability::DirectData, Io\Hooks\Capability::DirectAccept]);
$scheduler->target = StreamPollWeakHandle::create($a);
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($s, $a) {
    var_dump(socket_read($s, 10));
    var_dump(fread($a, 10));
});
$scheduler->spawn(function () use ($b, $s, $a) {
    usleep(20000);
    fwrite($b, "hello");
    usleep(20000);
    fwrite($b, "world");
});
$scheduler->loop();

// The Socket's read and the stream's read are the same kind of op on the stream's one pair
var_dump($scheduler->ops, $scheduler->adds);

// A select over an imported Socket waits on the stream too
$scheduler->spawn(function () use ($s) {
    $r = [$s]; $w = $e = null;
    var_dump(socket_select($r, $w, $e, 1), $r === [$s], socket_read($s, 10));
});
$scheduler->spawn(function () use ($b) {
    usleep(20000);
    fwrite($b, "more");
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
string(5) "hello"
string(5) "world"
array(2) {
  [0]=>
  string(29) "Recv reg StreamPollWeakHandle"
  [1]=>
  string(29) "Recv reg StreamPollWeakHandle"
}
array(1) {
  [0]=>
  string(25) "Read StreamPollWeakHandle"
}
int(1)
bool(true)
string(4) "more"
