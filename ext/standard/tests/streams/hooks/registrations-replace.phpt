--TEST--
IO hooks: replacing the provider ends its registrations without remove(), the next one sees add() again
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $log = [];
    public array $regs = [];
    public \Io\Poll\OperationQueue $q;

    public function __construct(public string $name)
    {
        $this->q = new \Io\Poll\OperationQueue();
        parent::__construct($this->q, [\Io\Hooks\Capability::EdgeRegistrations]);
    }

    public function run(\Io\Operation $op): \Io\Completion
    {
        $reg = $op->getRegistration();
        $this->log[] = "$this->name run " . substr($op::class, 13) . ($reg ? " on " . $reg->getEvent()->name : " unregistered");
        return parent::run($op);
    }

    public function add(\Io\Registration $registration): void
    {
        $this->regs[] = $registration;
        $this->log[] = "$this->name add " . $registration->getEvent()->name . " " . $registration->getTrigger()->name;
        parent::add($registration);
    }

    public function remove(\Io\Registration $registration): void
    {
        $this->log[] = "$this->name remove " . $registration->getEvent()->name;
        parent::remove($registration);
    }
}

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

$first = new Tracing('first');
Io\Hooks\set_hooks($first);
$first->spawn(function () use ($a) {
    $first = Io\Hooks\get_hooks();
    // Two waits on the read pair: one add()
    var_dump(fread($a, 10));
    var_dump(fread($a, 10));
    // The same object every time
    var_dump(count($first->regs), $first->regs[0]->isValid());
});
$first->spawn(function () use ($b) {
    fwrite($b, "1");
    usleep(1000);
    fwrite($b, "2");
});
$first->loop();

// Replaced with a registration in place: no remove(), the object is detached
$second = new Tracing('second');
Io\Hooks\set_hooks($second);
var_dump($first->regs[0]->isValid());
try {
    $first->regs[0]->getHandle();
} catch (Io\InvalidRegistrationException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $second->q->add($first->regs[0]);
} catch (Io\InvalidRegistrationException $e) {
    echo "queue: ", $e->getMessage(), "\n";
}
$second->spawn(function () use ($a) {
    var_dump(fread($a, 10));
    // The pair is removed before the descriptor closes
    fclose($a);
});
$second->spawn(function () use ($b) {
    fwrite($b, "3");
});
$second->loop();
Io\Hooks\set_hooks(null);

// A provider installed a second time sees add() again
[$c, $d] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
Io\Hooks\set_hooks($first);
$first->spawn(function () use ($c) {
    stream_set_timeout($c, 0, 10000);
    var_dump(fread($c, 10));
});
$first->loop();
Io\Hooks\set_hooks(null);
print_r($first->log);
print_r($second->log);
?>
--EXPECT--
string(1) "1"
string(1) "2"
int(1)
bool(true)
bool(false)
The registration has ended
queue: The registration has ended
string(1) "3"
bool(false)
Array
(
    [0] => first add Read Edge
    [1] => first run Recv on Read
    [2] => first run Timer unregistered
    [3] => first run Recv on Read
    [4] => first add Read Edge
    [5] => first run Recv on Read
)
Array
(
    [0] => second add Read Edge
    [1] => second run Recv on Read
    [2] => second remove Read
)
