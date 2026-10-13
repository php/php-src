--TEST--
TLS streams under IO hooks: a Recv orphaned on the ring marks the connection dead
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_io_hooks_orphan.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('io-hooks-orphan', $certFile);

final class GiveUp implements Io\Hooks\Hooks
{
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return Scheduler::capabilities($this->ring); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op);
        throw new RuntimeException("gave up on " . $op::class);
    }
}

$ring = new Io\Ring\Engine();
$scheduler = new Scheduler($ring);
Io\Hooks\set_hooks($scheduler);

$serverCtx = stream_context_create(['ssl' => ['local_cert' => $certFile]]);
$clientCtx = stream_context_create(['ssl' => [
    'verify_peer' => false,
    'verify_peer_name' => false,
]]);

$server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
$addr = stream_socket_get_name($server, false);

$conn = $client = null;
$scheduler->spawn(function () use ($server, &$conn) {
    $conn = stream_socket_accept($server, 5);
});
$scheduler->spawn(function () use ($addr, $clientCtx, &$client) {
    $client = stream_socket_client("tls://$addr", $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $clientCtx);
});
$scheduler->loop();
Io\Hooks\set_hooks(null);

// OpenSSL's read buffer is released once drained; data sent after the
// orphan lands in the ring's own buffer, if anywhere
fwrite($conn, "hello");
var_dump(fread($client, 5));

Io\Hooks\set_hooks(new GiveUp($ring));
try {
    fread($client, 100);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);
fwrite($conn, str_repeat("x", 20000));

$churn = [];
for ($i = 0; $i < 64; $i++) {
    $churn[] = str_repeat(chr(65 + $i % 26), 17000);
}
unset($churn);

// The stream stays frozen until the orphan settled
if ($ring->countPending()) {
    $ring->waitCompletions();
}

var_dump(fread($client, 100), feof($client));
fclose($client);
fclose($conn);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_io_hooks_orphan.pem.tmp');
?>
--EXPECT--
string(5) "hello"
gave up on Io\Operation\Recv
bool(false)
bool(true)
