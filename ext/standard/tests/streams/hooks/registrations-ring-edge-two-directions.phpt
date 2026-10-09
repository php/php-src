--TEST--
Io\Ring\Engine: a wait after a drain on a pair registered in both directions is answered by its own direction
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\Ring\Engine not available");
if (!in_array(Io\Hooks\Capability::EdgeRegistrations, (new Io\Ring\Engine())->getSupportedHookCapabilities(), true)) {
    die("skip the backend's multishot poll does not report edges");
}
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $log = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        $c = parent::run($op);
        if (!$op instanceof \Io\Operation\Timer) {
            $reg = $op->getRegistration();
            $this->log[] = substr($op::class, 13) . ($reg ? " on " . $reg->getTrigger()->name : "")
                . " " . $c->getStatus()->name . " " . implode("|", array_map(fn ($e) => $e->name, $c->getEvents()));
        }
        return $c;
    }

    public function add(\Io\Registration $registration): void
    {
        $this->log[] = "add " . $registration->getEvent()->name;
        parent::add($registration);
    }
}

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = 'tcp://' . stream_socket_get_name($server, false);
$ring = new Io\Ring\Engine();
$scheduler = new Tracing($ring, [Io\Hooks\Capability::EdgeRegistrations]);
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($addr) {
    // The connect registers the Write pair and the recvfrom the Read pair: the record covers both,
    // and the socket stays writable while the read waits, which must not answer the read's wait
    // (on IOCP a level poll of the undrained direction would complete at once and leave the wait
    // parked with nothing armed)
    $client = stream_socket_client($addr);
    stream_set_timeout($client, 2);
    var_dump(stream_socket_recvfrom($client, 10));
    var_dump(stream_socket_recvfrom($client, 10));
    fclose($client);
});
$scheduler->spawn(function () use ($server) {
    $peer = stream_socket_accept($server, 1);
    usleep(30000);
    fwrite($peer, "one");
    usleep(30000);
    fwrite($peer, "two");
    usleep(30000);
    fclose($peer);
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
print_r($scheduler->log);
?>
--EXPECT--
string(3) "one"
string(3) "two"
Array
(
    [0] => add Write
    [1] => Connect on Edge Ready Write
    [2] => add Read
    [3] => Poll on Edge Done Read
    [4] => Poll on Edge Done Read
)
