--TEST--
dtls:// client: peer name verification matches the certificate SAN
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

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_peer_name.pem.tmp';
$caFile   = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_peer_name-ca.pem.tmp';
$gen = new CertificateGenerator();
$gen->saveCaCert($caFile);
$gen->saveNewCertAsFileWithKey('dtls-server', $certFile, null, 'DNS:dtls.example.org');

function start_server($certFile) {
    $probe = stream_socket_server('udp://127.0.0.1:0', $e, $s, STREAM_SERVER_BIND);
    $port = (int) substr(strrchr(stream_socket_get_name($probe, false), ':'), 1);
    fclose($probe);

    $cmd = ['openssl', 's_server', '-dtls1_2', '-accept', (string) $port,
            '-cert', $certFile, '-key', $certFile];
    $proc = proc_open($cmd, [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']], $pipes);
    stream_set_blocking($pipes[1], false);
    stream_set_blocking($pipes[2], false);
    $deadline = microtime(true) + 10;
    $buf = '';
    while (microtime(true) < $deadline) {
        $r = [$pipes[1], $pipes[2]]; $w = $ex = [];
        if (stream_select($r, $w, $ex, 1)) {
            foreach ($r as $p) $buf .= (string) fread($p, 8192);
            if (strpos($buf, 'ACCEPT') !== false) break;
        }
    }
    return [$proc, $pipes, $port];
}

function stop_server($proc, $pipes) {
    proc_terminate($proc);
    foreach ($pipes as $p) {
        if (is_resource($p)) fclose($p);
    }
    proc_close($proc);
}

function connect_with_name($port, $caFile, $name) {
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => true,
        'cafile' => $caFile,
        'peer_name' => $name,
    ]]);
    $errno = $errstr = null;
    return @stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 10,
        STREAM_CLIENT_CONNECT, $ctx);
}

// 1) peer_name matches a SAN entry -> the handshake succeeds.
[$proc, $pipes, $port] = start_server($certFile);
$client = connect_with_name($port, $caFile, 'dtls.example.org');
var_dump($client !== false);
if ($client !== false) {
    fclose($client);
}
stop_server($proc, $pipes);

// 2) peer_name does not match -> the handshake is rejected.
[$proc, $pipes, $port] = start_server($certFile);
$client = connect_with_name($port, $caFile, 'wrong.example.org');
var_dump($client === false);
stop_server($proc, $pipes);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_peer_name.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_peer_name-ca.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
