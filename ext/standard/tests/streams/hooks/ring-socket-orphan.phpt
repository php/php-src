--TEST--
Io\Ring\Engine: a Socket whose op outlived its frame stays busy until the op settled, and closing it drains the op
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (PHP_OS_FAMILY === 'Windows') die("skip socket_create_pair() needs AF_UNIX");
?>
--FILE--
<?php
final class GiveUp implements Io\Hooks\Hooks
{
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op);
        throw new RuntimeException("gave up on " . $op::class);
    }
}

$ring = new Io\Ring\Engine();
foreach (['close', 'settle'] as $how) {
    socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair);
    [$a, $b] = $pair;
    Io\Hooks\set_hooks(new GiveUp($ring));
    try {
        socket_recv($a, $buf, 5, 0);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
    Io\Hooks\set_hooks(null);
    try {
        socket_recv($a, $buf, 5, MSG_DONTWAIT);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    if ($how === 'close') {
        socket_close($a);
        var_dump($ring->countPending());
    } else {
        while ($ring->countPending()) {
            $ring->waitCompletions();
        }
        socket_write($b, "hello");
        var_dump(socket_recv($a, $buf, 5, 0), $buf);
    }
}
?>
--EXPECT--
gave up on Io\Operation\Recv
Concurrent access to a socket
int(0)
gave up on Io\Operation\Recv
Concurrent access to a socket
int(5)
string(5) "hello"
