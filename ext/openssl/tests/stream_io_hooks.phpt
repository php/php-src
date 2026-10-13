--TEST--
TLS streams under IO hooks: the handshake, reads and writes run as operations
--EXTENSIONS--
openssl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_io_hooks.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('io-hooks', $certFile);

/* Records which operation classes the TLS layer submits */
class CountingScheduler extends Scheduler
{
    public array $seen = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        $class = substr(get_class($op), strlen('Io\\Operation\\'));
        $this->seen[$class] = ($this->seen[$class] ?? 0) + 1;
        return parent::run($op);
    }
}

$scheduler = new CountingScheduler();
Io\Hooks\set_hooks($scheduler);

$serverCtx = stream_context_create(['ssl' => ['local_cert' => $certFile]]);
$clientCtx = stream_context_create(['ssl' => [
    'verify_peer' => false,
    'verify_peer_name' => false,
]]);

$server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
$addr = stream_socket_get_name($server, false);

/* Server: the handshake happens inside accept, on this fiber */
$scheduler->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    var_dump($conn !== false);
    $data = fread($conn, 100);
    echo "server got: $data\n";
    fwrite($conn, "hello from server\n");
    /* Keep the connection quiet so the client read below times out */
    $line = fgets($conn);
    echo "server got: ", $line;
    fwrite($conn, str_repeat("x", 100000));
    fclose($conn);
});

/* Client: connect, handshake, exchange, then a timed-out read and a
 * non-blocking read */
$scheduler->spawn(function () use ($addr, $clientCtx) {
    $client = stream_socket_client("tls://$addr", $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $clientCtx);
    var_dump($client !== false);
    fwrite($client, "hello from client");
    $data = fread($client, 100);
    echo "client got: $data";

    stream_set_timeout($client, 0, 200000);
    $start = hrtime(true);
    var_dump(fread($client, 100));
    $meta = stream_get_meta_data($client);
    var_dump($meta['timed_out'], (hrtime(true) - $start) / 1e6 >= 150);

    stream_set_blocking($client, false);
    var_dump(fread($client, 100));
    var_dump(stream_socket_get_crypto_status($client) === STREAM_CRYPTO_STATUS_WANT_READ);
    stream_set_blocking($client, true);
    stream_set_timeout($client, 5);

    fwrite($client, "bye\n");
    $total = 0;
    while (!feof($client)) {
        $chunk = fread($client, 8192);
        if ($chunk === false) break;
        $total += strlen($chunk);
    }
    var_dump($total);
    fclose($client);
});

$scheduler->loop();

/* Send is only an operation when the syscall would block, so it depends on
 * the queue and the buffers; the others are certain */
foreach (['Accept', 'Connect', 'Recv'] as $class) {
    echo "$class: ", isset($scheduler->seen[$class]) ? "yes" : "no", "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_io_hooks.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
server got: hello from client
client got: hello from server
bool(false)
bool(true)
bool(true)
string(0) ""
bool(true)
server got: bye
int(100000)
Accept: yes
Connect: yes
Recv: yes
