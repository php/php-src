--TEST--
IO hooks: stream_select() on a partly read Edge-registered stream reports it readable
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
            $this->log[] = substr($op::class, 13) . ($reg ? " on " . $reg->getTrigger()->name : "") . " " . $c->getStatus()->name;
        }
        return $c;
    }

    public function add(\Io\Registration $registration): void
    {
        $this->log[] = "add " . $registration->getEvent()->name . " " . $registration->getTrigger()->name;
        parent::add($registration);
    }
}

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$scheduler = new Tracing(new Io\Poll\OperationQueue(), [Io\Hooks\Capability::EdgeRegistrations]);
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($a) {
    // Unbuffered: every fread() is a Recv op
    stream_set_read_buffer($a, 0);
    // The first wait registers the read pair; the writer's bytes complete it
    var_dump(fread($a, 8));
    // No new edge comes for the rest: the select checks at arm time, with the zero-timeout
    // poll that precedes its Any, so nothing reaches the provider
    $r = [$a]; $w = null; $e = null;
    var_dump(stream_select($r, $w, $e, 0));
    var_dump(fread($a, 100));
    // Drained: the next wait is a real one, answered by the next write
    var_dump(fread($a, 100));
    // Bytes arriving while nothing waits set the ready bit and the syscall takes them first, so
    // the next wait is answered from the record once (one spurious wakeup), then waits for real
    usleep(20000);
    var_dump(fread($a, 100));
    var_dump(fread($a, 100));
    var_dump(feof($a));
});
$scheduler->spawn(function () use ($b) {
    fwrite($b, "0123456789abcdefghij");
    usleep(10000);
    fwrite($b, "more");
    usleep(10000);
    fwrite($b, "late");
    usleep(30000);
    fclose($b);
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
// A backend that observes edges records the bytes that arrived while nothing waited, so one wait
// is answered from the record (one spurious wakeup); a level backend re-arms per wait and sees none
$spurious = (new Io\Poll\Context())->getBackend()->supportsEdgeTriggering() ? 1 : 0;
$ready = count(array_filter($scheduler->log, fn ($l) => $l === 'Recv on Edge Ready'));
var_dump($scheduler->log[0], $ready === 3 + $spurious, count($scheduler->log) === $ready + 1);
?>
--EXPECT--
string(8) "01234567"
int(1)
string(12) "89abcdefghij"
string(4) "more"
string(4) "late"
string(0) ""
bool(true)
string(13) "add Read Edge"
bool(true)
bool(true)
