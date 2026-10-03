--TEST--
IO hooks: a queue ignores the record another queue kept on a registration, and its own survives a provider swap
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

// Registers with one queue and waits on another: the wait's queue finds no record of its own on
// the registration and serves the pair one-shot
final class Split extends Scheduler
{
    public int $adds = 0;
    public function __construct(private \Io\Poll\OperationQueue $other, \Io\OperationQueue $queue)
    {
        parent::__construct($queue, [\Io\Hooks\Capability::EdgeRegistrations]);
    }
    public function add(\Io\Registration $registration): void
    {
        $this->adds++;
        $this->other->add($registration);
    }
    public function remove(\Io\Registration $registration): void
    {
        $this->other->remove($registration);
    }
}

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_read_buffer($a, 0);

$queue = new \Io\Poll\OperationQueue();
$split = new Split(new \Io\Poll\OperationQueue(), $queue);
Io\Hooks\set_hooks($split);
$split->spawn(function () use ($a) {
    var_dump(fread($a, 10), fread($a, 10));
});
$split->spawn(function () use ($b) {
    usleep(20000);
    fwrite($b, "one");
    usleep(20000);
    fwrite($b, "two");
});
$split->loop();
var_dump($split->adds);

// The same queue under a second provider: the pair is added again and the record found through it
$second = new Scheduler($queue, [\Io\Hooks\Capability::EdgeRegistrations]);
Io\Hooks\set_hooks($second);
$second->spawn(function () use ($a) {
    var_dump(fread($a, 10));
});
$second->spawn(function () use ($b) {
    usleep(20000);
    fwrite($b, "three");
});
$second->loop();

// And under a provider on a fresh queue, while the old one is gone
unset($queue, $split);
$third = new Scheduler(new \Io\Poll\OperationQueue(), [\Io\Hooks\Capability::EdgeRegistrations]);
Io\Hooks\set_hooks($third);
$third->spawn(function () use ($a) {
    var_dump(fread($a, 10));
    fclose($a);
});
$third->spawn(function () use ($b) {
    usleep(20000);
    fwrite($b, "four");
});
$third->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
string(3) "one"
string(3) "two"
int(1)
string(5) "three"
string(4) "four"
