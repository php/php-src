--TEST--
IO hooks: a cancellation that arrives after the completion leaves the bytes with the stream
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
final class Cancel
{
    public function __destruct() { throw new RuntimeException("cancelled"); }
}

final class CancelAfterCompletion implements Io\Hooks\Hooks
{
    public function __construct(private Io\OperationQueue $queue, private $peer) {}
    public function getCapabilities(): array { return $this->queue->getHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $c = $this->queue->submit($op);
        if ($this->peer) {
            fwrite($this->peer, "hello");
            $this->peer = null;
        }
        while ($c === null) {
            foreach ($this->queue->waitCompletions() as $done) {
                $c = $done;
            }
        }
        // The exception is pending when run() returns the completion
        $cancel = new Cancel();
        return $c;
    }
}

$queues = ['poll' => new Io\Poll\OperationQueue(), 'ring' => new Io\Ring\Engine()];
foreach ($queues as $name => $queue) {
    foreach (['buffered', 'unbuffered'] as $mode) {
        echo "-- $name $mode --\n";
        [$r, $w] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
        stream_set_timeout($r, 1);
        if ($mode === 'unbuffered') {
            stream_set_read_buffer($r, 0);
        }
        Io\Hooks\set_hooks(new CancelAfterCompletion($queue, $w));
        try {
            var_dump(fread($r, 5));
        } catch (RuntimeException $e) {
            echo $e->getMessage(), "\n";
        }
        Io\Hooks\set_hooks(null);
        fwrite($w, " world");
        var_dump(fread($r, 11));
    }
}
?>
--EXPECT--
-- poll buffered --
cancelled
string(11) "hello world"
-- poll unbuffered --
cancelled
string(11) "hello world"
-- ring buffered --
cancelled
string(11) "hello world"
-- ring unbuffered --
cancelled
bool(false)
