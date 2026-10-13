--TEST--
TLS streams under IO hooks: a cancelled Recv leaves the connection usable
--EXTENSIONS--
openssl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_io_hooks_cancel.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('io-hooks-cancel', $certFile);

class CancellingScheduler extends Scheduler
{
    public bool $cancelNextRecv = false;

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($this->cancelNextRecv && $op instanceof Io\Operation\Recv) {
            $this->cancelNextRecv = false;
            throw new RuntimeException("Recv cancelled");
        }
        return parent::run($op);
    }
}

$scheduler = new CancellingScheduler();
Io\Hooks\set_hooks($scheduler);

$serverCtx = stream_context_create(['ssl' => ['local_cert' => $certFile]]);
$clientCtx = stream_context_create(['ssl' => [
    'verify_peer' => false,
    'verify_peer_name' => false,
]]);

$server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    $line = fgets($conn);
    echo "server got: $line";
    fwrite($conn, "hello");
    $line = fgets($conn);
    echo "server got: $line";
    fclose($conn);
});

$scheduler->spawn(function () use ($scheduler, $addr, $clientCtx) {
    $client = stream_socket_client("tls://$addr", $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $clientCtx);
    $scheduler->cancelNextRecv = true;
    try {
        fread($client, 10);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump(feof($client));
    fwrite($client, "go\n");
    var_dump(fread($client, 10));
    fwrite($client, "bye\n");
    fclose($client);
});

$scheduler->loop();
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_io_hooks_cancel.pem.tmp');
?>
--EXPECT--
Recv cancelled
bool(false)
server got: go
string(5) "hello"
server got: bye
