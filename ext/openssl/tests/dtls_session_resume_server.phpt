--TEST--
dtls:// server: full session resumption via a userland cache (session_new_cb/get_cb)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_session_resume_server.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

// The session_new_cb/session_get_cb options let userland hold the session cache (keyed by
// session id). One server serves two connections and resumes the second from the session
// stored during the first.
$serverCode = <<<'CODE'
    $store = [];
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'dtls-resume',
        'session_new_cb' => function ($stream, $session) use (&$store) {
            $store[bin2hex($session->id)] = $session;
        },
        'session_get_cb' => function ($stream, $id) use (&$store) {
            return $store[bin2hex($id)] ?? null;
        },
    ]]);

    $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    // First connection: full handshake, session_new_cb stores the session.
    $peer1 = stream_socket_accept($server, 10);
    fwrite($peer1, strtoupper(fread($peer1, 64)));
    fread($peer1, 1);
    fclose($peer1);

    // Second connection: session_get_cb resumes.
    phpt_notify();
    $peer2 = stream_socket_accept($server, 10);
    $reused = stream_get_meta_data($peer2)['crypto']['session_reused'];
    fwrite($peer2, strtoupper(fread($peer2, 64)));
    fread($peer2, 1);
    fclose($peer2);
    fclose($server);

    phpt_notify(message: "reused=" . ($reused ? "yes" : "no") . " stored=" . count($store));
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    // First connection: capture the negotiated session.
    $session = null;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function ($stream, $s) use (&$session) { $session = $s; },
    ]]);
    $c1 = stream_socket_client("dtls://{{ ADDR }}", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx);
    fwrite($c1, "ping");
    $meta = stream_get_meta_data($c1)['crypto'];
    echo "captured=" . ($session instanceof Openssl\Session ? "yes" : "no") . "\n";
    echo "first_reused=" . ($meta['session_reused'] ? "yes" : "no") . "\n";
    fread($c1, 64);
    fclose($c1);

    // Reconnect presenting the session.
    phpt_wait();
    $ctx2 = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false, 'session_data' => $session]]);
    $c2 = stream_socket_client("dtls://{{ ADDR }}", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx2);
    fwrite($c2, "ping");
    echo "second_reused=" . (stream_get_meta_data($c2)['crypto']['session_reused'] ? "yes" : "no") . "\n";
    fread($c2, 64);
    fclose($c2);

    echo "server: " . trim(phpt_wait()) . "\n";
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_session_resume_server.pem.tmp');
?>
--EXPECT--
captured=yes
first_reused=no
second_reused=yes
server: reused=yes stored=1
