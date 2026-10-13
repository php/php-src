--TEST--
IO hooks: a Socket in an operation refuses another operation, a close and a select from run()
--EXTENSIONS--
sockets
--FILE--
<?php
socket_create_pair(PHP_OS_FAMILY === 'Windows' ? AF_INET : AF_UNIX, SOCK_STREAM, 0, $pair);
[$a, $b] = $pair;

class ConcurrentHook implements Io\Hooks\Hooks {
    public function __construct(private Socket $socket) {}
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion {
        var_dump($op instanceof Io\Operation\Recv, $op->getHandle()->getSocket());
        foreach (['socket_read' => fn () => socket_read($this->socket, 10),
                  'socket_write' => fn () => socket_write($this->socket, "x"),
                  'socket_close' => fn () => socket_close($this->socket),
                  'socket_shutdown' => fn () => socket_shutdown($this->socket),
                  'socket_select' => function () { $r = [$this->socket]; $w = $e = null; return socket_select($r, $w, $e, 0); }] as $name => $fn) {
            try {
                $fn();
                echo "$name: no error\n";
            } catch (Error $e) {
                echo "$name: ", $e->getMessage(), "\n";
            }
        }
        return $op->complete(Io\CompletionStatus::Timeout);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

Io\Hooks\set_hooks(new ConcurrentHook($a));
socket_set_option($a, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 1, 'usec' => 0]);
/* The timed out read fails like the blocking call: EAGAIN, on Windows the Winsock timeout code with its warning */
var_dump(@socket_read($a, 10), socket_last_error($a) === (PHP_OS_FAMILY === 'Windows' ? SOCKET_ETIMEDOUT : SOCKET_EAGAIN));
// Open again after the operation
var_dump(SocketPollWeakHandle::create($a)->getSocket() === $a);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(true)
NULL
socket_read: Concurrent access to a socket
socket_write: Concurrent access to a socket
socket_close: Concurrent access to a socket
socket_shutdown: Concurrent access to a socket
socket_select: Concurrent access to a socket
bool(false)
bool(true)
bool(true)
