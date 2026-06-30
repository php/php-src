--TEST--
dtls:// client: DTLS 1.2 handshake and data round trip against openssl s_server
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
if (substr(PHP_OS, 0, 3) === 'WIN') die('skip not for Windows');
exec('openssl version', $out, $code);
if ($code !== 0) die('skip openssl binary not available');
?>
--FILE--
<?php
include 'CertificateGenerator.inc';

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_basic.pem.tmp';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

// Grab a free UDP port, then hand it to s_server.
$probe = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
$port = (int) substr(strrchr(stream_socket_get_name($probe, false), ':'), 1);
fclose($probe);

// Plain relay mode: data from the client is printed to s_server's stdout, and
// s_server's stdin is sent to the client.
$cmd = ['openssl', 's_server', '-dtls1_2', '-accept', (string) $port,
        '-cert', $certFile, '-key', $certFile];
$descriptors = [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']];
$server = proc_open($cmd, $descriptors, $pipes);
if (!is_resource($server)) {
    die("skip-like: could not start s_server\n");
}

// s_server prints "ACCEPT" once it is bound and listening; wait for it so the
// client does not fire its ClientHello at an unbound port.
stream_set_blocking($pipes[1], false);
stream_set_blocking($pipes[2], false);
$ready = false;
$deadline = microtime(true) + 10;
$buf = '';
while (microtime(true) < $deadline) {
    $r = [$pipes[1], $pipes[2]]; $w = $e = [];
    if (stream_select($r, $w, $e, 1)) {
        foreach ($r as $pipe) {
            $buf .= (string) fread($pipe, 8192);
        }
        if (strpos($buf, 'ACCEPT') !== false) { $ready = true; break; }
    }
}

// Self-signed s_server cert: skip verification for this round-trip scenario.
$ctx = stream_context_create(['ssl' => ['verify_peer' => false]]);
$client = $ready
    ? stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx)
    : false;
var_dump($client !== false);

if ($client !== false) {
    var_dump(stream_get_meta_data($client)['crypto']['protocol'] === 'DTLSv1.2');
    stream_set_timeout($client, 10);

    // client -> server: our datagram should show up on s_server's stdout.
    fwrite($client, "PING\n");
    // server -> client: feed s_server's stdin, it relays to the client.
    fwrite($pipes[0], "PONG\n");

    // stream_select() on the dtls:// stream exercises the transport cast op.
    $r = [$client]; $w = $e = [];
    var_dump(stream_select($r, $w, $e, 10) === 1);
    var_dump(rtrim((string) fread($client, 8192)) === 'PONG');

    $srvOut = '';
    $deadline = microtime(true) + 5;
    while (microtime(true) < $deadline && strpos($srvOut, 'PING') === false) {
        $srvOut .= (string) fread($pipes[1], 8192);
        usleep(50000);
    }
    var_dump(strpos($srvOut, 'PING') !== false);

    // A read with nothing pending times out; the metadata reflects it.
    stream_set_timeout($client, 0, 200000);
    fread($client, 8192);
    var_dump(stream_get_meta_data($client)['timed_out']);

    // A non-blocking read with nothing pending returns '' (would block), not false.
    stream_set_blocking($client, false);
    var_dump(fread($client, 8192));

    fclose($client);
}

proc_terminate($server);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($server);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_basic.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(0) ""
