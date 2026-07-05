--TEST--
dtls:// client: captures a resumable session and accepts session_data
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
// Note: a full "resumed handshake" round trip (session_reused === true) needs a
// server that accepts two connections sharing one session cache / ticket key.
// openssl s_server does not loop for DTLS, and the dtls:// server is single-peer
// (one accept per bind), so this test verifies the client-side plumbing: the
// negotiated session is captured, is resumable, and session_data is accepted
// (graceful full handshake when the peer has no matching session).
include 'CertificateGenerator.inc';
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_session_resume.pem.tmp';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

function start_s_server($certFile, &$port) {
    $probe = stream_socket_server('udp://127.0.0.1:0', $e, $s, STREAM_SERVER_BIND);
    $port = (int) substr(strrchr(stream_socket_get_name($probe, false), ':'), 1);
    fclose($probe);
    $cmd = ['openssl', 's_server', '-dtls1_2', '-accept', (string) $port,
            '-cert', $certFile, '-key', $certFile, '-quiet'];
    $proc = proc_open($cmd, [['pipe','r'], ['pipe','w'], ['pipe','w']], $pipes);
    $deadline = microtime(true) + 5;
    while (microtime(true) < $deadline) {
        usleep(100000);
        $t = @stream_socket_client("udp://127.0.0.1:$port", $e, $s, 0.2);
        if ($t) { fclose($t); break; }
    }
    return [$proc, $pipes];
}

function stop($proc, $pipes) {
    proc_terminate($proc);
    foreach ($pipes as $p) if (is_resource($p)) fclose($p);
    proc_close($proc);
}

// 1) Full handshake: capture the negotiated session.
[$proc, $pipes] = start_s_server($certFile, $port);
$ctx = stream_context_create(['ssl' => ['verify_peer' => false]]);
$c = stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
$crypto = stream_get_meta_data($c)['crypto'];
var_dump($crypto['session_reused']);
var_dump($crypto['session'] instanceof Openssl\Session);
var_dump($crypto['session']->isResumable());
var_dump(strlen($crypto['session']->export()) > 0);
$session = $crypto['session'];
fclose($c);
stop($proc, $pipes);

// 2) session_data is accepted; against a fresh server it falls back to a full
//    handshake without error.
[$proc, $pipes] = start_s_server($certFile, $port);
$ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'session_data' => $session]]);
$c = stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
var_dump($c !== false);
fclose($c);
stop($proc, $pipes);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_session_resume.pem.tmp');
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
