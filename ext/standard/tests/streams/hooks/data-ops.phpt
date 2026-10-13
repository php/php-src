--TEST--
IO hooks: socket reads, writes, accepts and connects reach the provider as data operations
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $seen = [];
    public array $problems = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        $this->seen[$op::class] = ($this->seen[$op::class] ?? 0) + 1;
        $check = fn (bool $ok, string $what) => $ok || $this->problems[] = $op::class . ': ' . $what;
        if ($op instanceof \Io\Operation\Recv || $op instanceof \Io\Operation\Send) {
            $check($op->getLength() > 0, 'length');
            $check($op->getFlags() === 0, 'flags');
            $check($op->getHandle() instanceof \Io\Poll\WeakHandle, 'handle');
            $check($op->getEvents() === [$op instanceof \Io\Operation\Recv ? \Io\Poll\Event::Read : \Io\Poll\Event::Write], 'events');
        } elseif ($op instanceof \Io\Operation\Accept) {
            $check($op->getHandle() instanceof \Io\Poll\WeakHandle, 'handle');
            $check($op->getEvents() === [\Io\Poll\Event::Read], 'events');
            $check($op->getTimeout() !== null, 'timeout');
        } elseif ($op instanceof \Io\Operation\Connect) {
            $check(str_starts_with($op->getAddress(), '127.0.0.1:'), 'address ' . $op->getAddress());
            $check($op->getEvents() === [\Io\Poll\Event::Write], 'events');
        }
        return parent::run($op);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    // Read before the client writes, so the read has to wait
    $line = fgets($conn);
    fwrite($conn, strtoupper($line));
    fclose($conn);
});
$scheduler->spawn(function () use ($addr) {
    $c = stream_socket_client("tcp://$addr");
    usleep(10000);
    fwrite($c, "hello\n");
    echo fgets($c);
    fclose($c);
});
$scheduler->loop();

var_dump(isset($scheduler->seen[Io\Operation\Accept::class]), isset($scheduler->seen[Io\Operation\Recv::class]));
var_dump($scheduler->problems);
?>
--EXPECT--
HELLO
bool(true)
bool(true)
array(0) {
}
