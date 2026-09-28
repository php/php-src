--TEST--
IO hooks: datagram waits are readiness operations bounded by the socket timeouts
--EXTENSIONS--
sockets
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $ops = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        if (!$op instanceof \Io\Operation\Timer) {
            $this->ops[] = substr($op::class, 13) . ' ' . get_class($op->getHandle())
                . ' ' . implode(',', array_map(fn ($e) => $e->name, $op->getEvents()));
        }
        return parent::run($op);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

$srv = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($srv, '127.0.0.1', 0);
socket_getsockname($srv, $addr, $port);

$scheduler->spawn(function () use ($srv) {
    var_dump(socket_recvfrom($srv, $buf, 100, 0, $from, $fromPort), $buf);
    // Nothing more arrives: the wait ends with the receive timeout, as the blocking call does
    socket_set_option($srv, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 0, 'usec' => 50000]);
    $start = hrtime(true);
    var_dump(@socket_recvfrom($srv, $buf, 100, 0, $from, $fromPort), socket_last_error($srv) === SOCKET_EAGAIN);
    var_dump((hrtime(true) - $start) / 1e6 >= 40);
    // A non-blocking Socket does not wait
    socket_set_nonblock($srv);
    var_dump(@socket_recvfrom($srv, $buf, 100, 0, $from, $fromPort), socket_last_error($srv) === SOCKET_EAGAIN);
});
$scheduler->spawn(function () use ($addr, $port) {
    usleep(30000);
    $c = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    var_dump(socket_sendto($c, "ping", 4, 0, $addr, $port));
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
print_r($scheduler->ops);
?>
--EXPECT--
int(4)
int(4)
string(4) "ping"
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Array
(
    [0] => Poll SocketPollWeakHandle Read
    [1] => Poll SocketPollWeakHandle Read
)
