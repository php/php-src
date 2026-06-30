--TEST--
dtls:// client: an encrypted local_pk is unlocked with the passphrase option
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

$serverCert = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_local_pk_passphrase-server.pem.tmp';
$clientCert = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_local_pk_passphrase-client-cert.pem.tmp';
$clientKey  = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_local_pk_passphrase-client-key.pem.tmp';

(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $serverCert);

// A client certificate with a passphrase-encrypted private key in a separate file.
$key = openssl_pkey_new(['private_key_bits' => 2048, 'private_key_type' => OPENSSL_KEYTYPE_RSA]);
$csr = openssl_csr_new(['commonName' => 'dtls-client'], $key);
$cert = openssl_csr_sign($csr, null, $key, 365);
openssl_x509_export($cert, $certPem);
openssl_pkey_export($key, $keyPem, 'secret');
file_put_contents($clientCert, $certPem);
file_put_contents($clientKey, $keyPem);

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

function connect_with_passphrase($port, $clientCert, $clientKey, $passphrase) {
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'local_cert' => $clientCert,
        'local_pk' => $clientKey,
        'passphrase' => $passphrase,
    ]]);
    $errno = $errstr = null;
    return @stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 10,
        STREAM_CLIENT_CONNECT, $ctx);
}

// 1) correct passphrase -> the key is loaded and the handshake succeeds.
[$proc, $pipes, $port] = start_server($serverCert);
$client = connect_with_passphrase($port, $clientCert, $clientKey, 'secret');
var_dump($client !== false);
if ($client !== false) {
    fclose($client);
}
stop_server($proc, $pipes);

// 2) wrong passphrase -> the key cannot be loaded and the connection fails.
[$proc, $pipes, $port] = start_server($serverCert);
$client = connect_with_passphrase($port, $clientCert, $clientKey, 'wrong');
var_dump($client === false);
stop_server($proc, $pipes);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_local_pk_passphrase-server.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_local_pk_passphrase-client-cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_local_pk_passphrase-client-key.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
