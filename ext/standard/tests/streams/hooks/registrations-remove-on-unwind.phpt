--TEST--
IO hooks: a stream freed by an unwinding exception still ends its registrations with remove()
--INI--
zend.exception_ignore_args=1
--SKIPIF--
<?php
/* The registrations of the pairs are the point: IO_HOOKS_CAPS may leave them out */
include __DIR__ . '/scheduler.inc';
$caps = Scheduler::capabilities(Scheduler::defaultQueue());
if (!in_array(Io\Hooks\Capability::EdgeRegistrations, $caps, true) && !in_array(Io\Hooks\Capability::LevelRegistrations, $caps, true)) {
    die('skip the provider takes no registrations');
}
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $adds = [];
    public array $removes = [];
    public bool $throw = false;

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($this->throw && $op instanceof \Io\Operation\Recv) {
            $this->throw = false;
            throw new RuntimeException("cancelled");
        }
        return parent::run($op);
    }

    public function add(\Io\Registration $registration): void
    {
        $this->adds[] = $registration->getEvent()->name;
        parent::add($registration);
    }

    public function remove(\Io\Registration $registration): void
    {
        $this->removes[] = $registration->getEvent()->name;
        parent::remove($registration);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = 'tcp://' . stream_socket_get_name($server, false);

// The connect registers the write pair and the read the read pair; the provider throws from the
// read, and the unwinding fiber frees the client stream with the exception pending
$scheduler->spawn(function () use ($addr, $scheduler) {
    $client = stream_socket_client($addr);
    $scheduler->throw = true;
    try {
        fread($client, 10);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
        throw $e;
    }
});
try {
    $scheduler->loop();
} catch (RuntimeException $e) {
    echo "unwound\n";
}
sort($scheduler->adds);
sort($scheduler->removes);
var_dump($scheduler->adds, $scheduler->removes);

// The next socket, on the same descriptor number as likely as not, registers and waits afresh
fclose($server);
$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = 'tcp://' . stream_socket_get_name($server, false);
$scheduler->spawn(function () use ($addr, $server) {
    $client = stream_socket_client($addr);
    $peer = stream_socket_accept($server, 1);
    fwrite($peer, "x");
    var_dump(fread($client, 10));
});
$scheduler->loop();
var_dump(count($scheduler->adds) >= 3, count($scheduler->removes) >= 3);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
cancelled
unwound
array(2) {
  [0]=>
  string(4) "Read"
  [1]=>
  string(5) "Write"
}
array(2) {
  [0]=>
  string(4) "Read"
  [1]=>
  string(5) "Write"
}
string(1) "x"
bool(true)
bool(true)
