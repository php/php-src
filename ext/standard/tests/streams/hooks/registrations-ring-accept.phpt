--TEST--
Io\Ring\Engine: a burst of connections under DirectAccept is served in order of arrival from a multishot accept
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (!in_array(Io\Hooks\Capability::DirectAccept, (new Io\Ring\Engine())->getSupportedHookCapabilities(), true)) {
    die("skip the backend does not support DirectAccept");
}
if (!in_array(Io\Hooks\Capability::EdgeRegistrations, (new Io\Ring\Engine())->getSupportedHookCapabilities(), true)) {
    die("skip the listener's pair needs EdgeRegistrations");
}
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public int $accepts = 0;
    public int $registered = 0;
    public array $regs = [];
    public $listener = null;

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Accept) {
            $this->accepts++;
            if ($op->getRegistration() !== null) {
                $this->registered++;
            }
        }
        return parent::run($op);
    }

    public function add(\Io\Registration $registration): void
    {
        // Only the listener's pairs; the clients register theirs too. The handle is the
        // stream's one object, and the stream is frozen by the accept, so getStream() is null
        if ($registration->getHandle() === StreamPollWeakHandle::create($this->listener)) {
            $this->regs[] = $registration->getEvent()->name . " " . $registration->getTrigger()->name;
        }
        parent::add($registration);
    }
}

$n = 40;
$ring = new Io\Ring\Engine();
$scheduler = new Tracing($ring, [Io\Hooks\Capability::EdgeRegistrations, Io\Hooks\Capability::DirectAccept]);
Io\Hooks\set_hooks($scheduler);

$ctx = stream_context_create(['socket' => ['backlog' => 128]]);
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
$addr = stream_socket_get_name($server, false);
$scheduler->listener = $server;

$order = [];
$peers = true;
$scheduler->spawn(function () use ($server, $n, &$order, &$peers) {
    // The first accept arms the multishot and parks; the burst then fills the buffer past its
    // cap, so the multishot is let go and comes back once the takes brought it below half
    for ($i = 0; $i < $n; $i++) {
        $conn = stream_socket_accept($server, 5, $peer);
        $peers = $peers && $peer !== '' && $peer === stream_socket_get_name($conn, true);
        $order[] = (int) fread($conn, 10);
        fclose($conn);
    }
});
for ($i = 0; $i < $n; $i++) {
    $scheduler->spawn(function () use ($addr, $i) {
        $c = stream_socket_client("tcp://$addr", $errno, $errstr, 5);
        fwrite($c, (string) $i);
        // Kept open until the server read it
        fread($c, 1);
    });
}
$scheduler->loop();

var_dump(count($order), $order === range(0, $n - 1), $peers);
var_dump($scheduler->accepts, $scheduler->registered, $scheduler->regs);
// Accepts served from the buffer complete at submit: the accepting fiber takes them without a pass
var_dump($scheduler->inline >= 20);

// A parked accept keeps its deadline without an entry of its own
$scheduler->spawn(function () use ($server) {
    $start = hrtime(true);
    var_dump(@stream_socket_accept($server, 0.2));
    var_dump((hrtime(true) - $start) / 1e9 >= 0.15);
});
$scheduler->loop();

// The multishot is armed with no waiter: not pending
var_dump($ring->countPending());

// accept_multishot => false keeps a listener unregistered: its accepts stay one-shot
$shared = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
    stream_context_create(['socket' => ['accept_multishot' => false]]));
$saddr = stream_socket_get_name($shared, false);
$scheduler->listener = $shared;
$before = [$scheduler->accepts, $scheduler->registered, count($scheduler->regs)];
$scheduler->spawn(function () use ($shared) {
    $conn = stream_socket_accept($shared, 5);
    var_dump(fread($conn, 10));
});
$scheduler->spawn(function () use ($saddr) {
    $c = stream_socket_client("tcp://$saddr", $errno, $errstr, 5);
    fwrite($c, "shared");
    fread($c, 1);
});
$scheduler->loop();
var_dump($scheduler->accepts - $before[0], $scheduler->registered - $before[1], count($scheduler->regs) - $before[2]);

// Closing the listener removes the pair and lets the multishot go
fclose($server);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
int(40)
bool(true)
bool(true)
int(40)
int(40)
array(1) {
  [0]=>
  string(9) "Read Edge"
}
bool(true)
bool(false)
bool(true)
int(0)
string(6) "shared"
int(1)
int(0)
int(0)
