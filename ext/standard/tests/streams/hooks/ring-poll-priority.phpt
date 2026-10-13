--TEST--
Io\Ring\Engine: stream_select() reports the except set although ior has no priority event
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (!Io\Poll\Backend::Auto->supportsPriority()) die("skip the poll backend has no priority event");
?>
--FILE--
<?php
final class Waiting implements Io\Hooks\Hooks
{
    public $peer;
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $c = $this->ring->submit($op);
        // The urgent byte arrives once the wait is submitted
        if ($this->peer) {
            $peer = $this->peer;
            $this->peer = null;
            Io\Hooks\set_hooks(null);
            stream_socket_sendto($peer, "!", STREAM_OOB);
            Io\Hooks\set_hooks($this);
        }
        while ($c === null) {
            foreach ($this->ring->waitCompletions() as $done) {
                $c = $done;
            }
        }
        return $c;
    }
}

$server = stream_socket_server('tcp://127.0.0.1:0');
$client = stream_socket_client('tcp://' . stream_socket_get_name($server, false));
$conn = stream_socket_accept($server);
$provider = new Waiting(new Io\Ring\Engine());
$provider->peer = $client;
Io\Hooks\set_hooks($provider);
$r = [];
$w = [];
$e = [$conn];
var_dump(stream_select($r, $w, $e, 5), count($e));
Io\Hooks\set_hooks(null);
?>
--EXPECT--
int(1)
int(1)
