--TEST--
IO hooks: socket functions issue operations that carry the Socket's handle and register its pairs
--EXTENSIONS--
sockets
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $ops = [];
    public array $adds = [];
    public array $pairs = [];
    public array $removes = [];
    public bool $handlesOk = true;

    public function run(\Io\Operation $op): \Io\Completion
    {
        if (!$op instanceof \Io\Operation\Timer) {
            $h = $op->getHandle();
            $reg = $op->getRegistration();
            $this->ops[substr($op::class, 13)] = true;
            // The Socket's one handle, which hands nothing out while the op runs; a wait on a
            // registered pair carries the pair's record with the same handle
            $this->handlesOk = $this->handlesOk && $h instanceof SocketPollWeakHandle && $h->isValid()
                && $h->getSocket() === null && $op->getHandle() === $h
                && ($reg === null || $reg->getHandle() === $h);
        }
        return parent::run($op);
    }

    public function add(\Io\Registration $r): void
    {
        $this->adds[] = $r;
        $this->pairs[] = $r->getEvent()->name . ' ' . get_class($r->getHandle()) . ' ' . $r->getTrigger()->name;
        parent::add($r);
    }

    public function remove(\Io\Registration $r): void
    {
        $this->removes[] = $r;
        parent::remove($r);
    }
}

// Registrations on both queues, direct ops where the queue performs them
$scheduler = new Tracing(null, [Io\Hooks\Capability::EdgeRegistrations, Io\Hooks\Capability::LevelRegistrations,
    Io\Hooks\Capability::DirectData, Io\Hooks\Capability::DirectAccept]);
Io\Hooks\set_hooks($scheduler);

$server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($server, '127.0.0.1', 0);
socket_listen($server);
socket_getsockname($server, $addr, $port);
$serverHandle = SocketPollWeakHandle::create($server);

$scheduler->spawn(function () use ($server, $serverHandle, $scheduler) {
    $conn = socket_accept($server);
    var_dump($conn instanceof Socket);
    var_dump(socket_read($conn, 10));
    var_dump(socket_recv($conn, $buf, 3, MSG_WAITALL), $buf);
    var_dump(socket_write($conn, "reply"));
    // The listener's handle is the object its accept carried, open again after the op
    var_dump($serverHandle === SocketPollWeakHandle::create($server), $serverHandle->getSocket() === $server);
    socket_close($conn);
});
$client = [];
$scheduler->spawn(function () use ($addr, $port, &$client) {
    $c = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    $client[] = socket_connect($c, $addr, $port);
    usleep(20000);
    $client[] = socket_send($c, "hello", 5, 0);
    usleep(20000);
    $client[] = socket_write($c, "abc");
    $client[] = socket_read($c, 10);
    socket_close($c);
});
$scheduler->loop();
var_dump($client);

var_dump(isset($scheduler->ops['Accept']), isset($scheduler->ops['Recv']), $scheduler->handlesOk);

// The listener's registration goes with its close; its handle answers invalid
socket_close($server);
var_dump($serverHandle->isValid(), $serverHandle->getSocket());

// Every pair was registered on a Socket handle, as Edge, and removed before its close
var_dump(count($scheduler->adds) >= 2, count($scheduler->adds) === count($scheduler->removes));
var_dump(array_all($scheduler->removes, fn ($r) => in_array($r, $scheduler->adds, true)));
var_dump(array_unique(array_map(fn ($p) => substr($p, strpos($p, ' ') + 1), $scheduler->pairs)) === ['SocketPollWeakHandle Edge']);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(true)
string(5) "hello"
int(3)
string(3) "abc"
int(5)
bool(true)
bool(true)
array(4) {
  [0]=>
  bool(true)
  [1]=>
  int(5)
  [2]=>
  int(3)
  [3]=>
  string(5) "reply"
}
bool(true)
bool(true)
bool(true)
bool(false)
NULL
bool(true)
bool(true)
bool(true)
bool(true)
