--TEST--
Io\Ring\Engine: a read the provider cancelled after it took bytes keeps them for the stream
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
function settle(Io\Ring\Engine $ring): void
{
    while ($ring->countPending()) {
        $ring->waitCompletions();
    }
}

function pair()
{
    $pair = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    stream_set_timeout($pair[0], 1);
    return $pair;
}

$ring = new Io\Ring\Engine();

echo "-- cancelled in flight --\n";
final class CancelIt implements Io\Hooks\Hooks
{
    public function __construct(private Io\Ring\Engine $ring, private $peer) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op);
        Io\Hooks\set_hooks(null);
        fwrite($this->peer, "hello");
        $until = hrtime(true) + 20000000;
        while (hrtime(true) < $until);
        $this->ring->cancel($op);
        return $op->complete(Io\CompletionStatus::Cancelled);
    }
}
[$r, $w] = pair();
Io\Hooks\set_hooks(new CancelIt($ring, $w));
var_dump(@fread($r, 5));
Io\Hooks\set_hooks(null);
// The backend may still fill the buffer: frozen until the op settled
try {
    stream_get_meta_data($r);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
settle($ring);
fwrite($w, " world");
var_dump(fread($r, 11));

echo "-- cancelled once reaped --\n";
final class Parked implements Io\Hooks\Hooks
{
    public array $ops = [];
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op, Fiber::getCurrent());
        $this->ops[spl_object_id(Fiber::getCurrent())] = $op;
        return Fiber::suspend();
    }
}
$provider = new Parked($ring);
$pairs = [];
$fibers = [];
for ($i = 0; $i < 2; $i++) {
    $pairs[$i] = pair();
    $r = $pairs[$i][0];
    $fibers[$i] = new Fiber(function () use ($r) { return @fread($r, 5); });
}
Io\Hooks\set_hooks($provider);
foreach ($fibers as $f) {
    $f->start();
}
Io\Hooks\set_hooks(null);
foreach ($pairs as [$r, $w]) {
    fwrite($w, "hello");
}
$until = hrtime(true) + 50000000;
while (hrtime(true) < $until);

// Both are reaped, one is delivered and the other cancelled
[$c] = $ring->waitCompletions(null, 1);
$done = $c->getData();
Io\Hooks\set_hooks($provider);
$done->resume($c);
foreach ($fibers as $i => $f) {
    if ($f !== $done) {
        $op = $provider->ops[spl_object_id($f)];
        $ring->cancel($op);
        $f->resume($op->complete(Io\CompletionStatus::Cancelled));
        $victim = $i;
    }
}
Io\Hooks\set_hooks(null);
settle($ring);
var_dump($done->getReturn(), $fibers[$victim]->getReturn());
fwrite($pairs[$victim][1], " world");
var_dump(fread($pairs[$victim][0], 11));
?>
--EXPECT--
-- cancelled in flight --
bool(false)
Concurrent access to a stream
string(11) "hello world"
-- cancelled once reaped --
string(5) "hello"
bool(false)
string(11) "hello world"
