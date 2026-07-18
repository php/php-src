--TEST--
dtls:// client: a malformed peer_fingerprint is rejected with a warning
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

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_peer_fingerprint_invalid.pem.tmp';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

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

// Connect with the given peer_fingerprint and return [connected, captured warnings].
function connect_fingerprint($port, $fingerprint) {
    $warnings = [];
    set_error_handler(function ($no, $str) use (&$warnings) { $warnings[] = $str; return true; });
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'peer_fingerprint' => $fingerprint,
    ]]);
    $client = stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx);
    restore_error_handler();
    if ($client !== false) {
        fclose($client);
    }
    return [$client !== false, $warnings];
}

// A string fingerprint that is not a md5 (32) or sha1 (40) hex length.
[$proc, $pipes, $port] = start_server($certFile);
[$ok, $warnings] = connect_fingerprint($port, 'not-a-real-hash');
var_dump($ok);
var_dump((bool) array_filter($warnings, fn($w) => str_contains($w, 'md5 or sha1 hash')));
stop_server($proc, $pipes);

// An array without an [algo => hash] shape.
[$proc, $pipes, $port] = start_server($certFile);
[$ok, $warnings] = connect_fingerprint($port, ['sha256']);
var_dump($ok);
var_dump((bool) array_filter($warnings, fn($w) => str_contains($w, '[algo => fingerprint]')));
stop_server($proc, $pipes);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_peer_fingerprint_invalid.pem.tmp');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
