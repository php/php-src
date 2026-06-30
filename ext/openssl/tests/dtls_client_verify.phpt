--TEST--
dtls:// client: peer certificate verification (verify_peer)
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

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_verify.pem.tmp';
$caFile   = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_verify-ca.pem.tmp';
$gen = new CertificateGenerator();
$gen->saveCaCert($caFile);
$gen->saveNewCertAsFileWithKey('dtls-server', $certFile);

// Start a DTLS s_server on a fresh UDP port; returns [proc, pipes, port].
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

// 1) verify_peer with the matching CA -> the handshake succeeds.
[$proc, $pipes, $port] = start_server($certFile);
$ctx = stream_context_create(['ssl' => [
    'verify_peer' => true,
    'verify_peer_name' => false,
    'cafile' => $caFile,
]]);
$client = stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx);
var_dump($client !== false);
if ($client !== false) {
    fclose($client);
}
stop_server($proc, $pipes);

// 2) verify_peer without the CA -> the handshake fails on verification.
[$proc, $pipes, $port] = start_server($certFile);
$ctx = stream_context_create(['ssl' => [
    'verify_peer' => true,
    'verify_peer_name' => false,
]]);
$client = @stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx);
var_dump($client === false);
var_dump(str_contains($errstr, 'verify'));
stop_server($proc, $pipes);

?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_verify.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_verify-ca.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
