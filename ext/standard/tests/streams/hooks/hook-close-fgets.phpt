--TEST--
IO hooks: fclose() from run() throws the concurrent access error
--FILE--
<?php

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);
$client = stream_socket_client("tcp://$addr");

class CloseOnceHooks implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion {
        fclose($op->getHandle()->getStream());
        return $op->completeReady($op->getEvents());
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

Io\Hooks\set_hooks(new CloseOnceHooks());
try {
    fgets($client);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Concurrent access to a stream
