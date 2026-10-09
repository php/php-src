--TEST--
Io\Ring\Engine: waits after a drain on an Edge pair are served from one multishot poll
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (!in_array(Io\Hooks\Capability::EdgeRegistrations, (new Io\Ring\Engine())->getSupportedHookCapabilities(), true)) {
    die("skip the backend's multishot poll does not report edges");
}
if ((new Io\Ring\Engine())->getBackend() === Io\Ring\Backend::Iocp) {
    // feof()'s zero-timeout poll: ior's IOCP poll does not check readiness at submit, so its
    // linked zero timeout wins and a closed peer is reported one read late
    die("skip a zero-timeout poll times out on IOCP before the poller reports");
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

    public function remove(\Io\Registration $registration): void
    {
        $this->log[] = "remove " . $registration->getEvent()->name;
        parent::remove($registration);
    }
}

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$ring = new Io\Ring\Engine();
$scheduler = new Tracing($ring, [Io\Hooks\Capability::EdgeRegistrations]);
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($a) {
    // recvfrom waits with a Poll op after EAGAIN: the first one registers the pair and arms
    var_dump(stream_socket_recvfrom($a, 100));
    var_dump(stream_socket_recvfrom($a, 100));
    // Bytes that arrived while nothing waited set the ready bits; the syscall takes them first,
    // so the next wait is answered from the record once, then waits for real
    usleep(20000);
    var_dump(stream_socket_recvfrom($a, 100));
    var_dump(stream_socket_recvfrom($a, 100));
    // The peer's close is an edge too
    var_dump(stream_socket_recvfrom($a, 100), feof($a));
    // Removed before the descriptor closes
    fclose($a);
});
$scheduler->spawn(function () use ($b) {
    usleep(5000);
    fwrite($b, "one");
    usleep(5000);
    fwrite($b, "two");
    usleep(5000);
    fwrite($b, "three");
    usleep(30000);
    fwrite($b, "four");
    usleep(30000);
    fclose($b);
});
$scheduler->loop();

// An armed multishot with no waiter is not pending: nothing can complete
var_dump($ring->countPending());
try {
    $ring->waitCompletions();
} catch (Io\IoException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);
print_r($scheduler->log);
?>
--EXPECT--
string(3) "one"
string(3) "two"
string(5) "three"
string(4) "four"
string(0) ""
bool(true)
int(0)
No operation can complete: nothing pending has a descriptor or a deadline
Array
(
    [0] => add Read
    [1] => Poll on Edge Done Read
    [2] => Poll on Edge Done Read
    [3] => Poll on Edge Done Read
    [4] => Poll on Edge Done Read
    [5] => Poll on Edge Done Read|HangUp
    [6] => remove Read
)
