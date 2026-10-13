--TEST--
IO hooks: socket_accept() whose operation throws returns no freed Socket
--EXTENSIONS--
sockets
--FILE--
<?php
Io\Hooks\set_hooks(new class implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return []; }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        throw new RuntimeException("cancelled " . $op::class);
    }
});

$server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($server, '127.0.0.1', 0);
socket_listen($server);

try {
    var_dump(socket_accept($server));
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
cancelled Io\Operation\Accept
