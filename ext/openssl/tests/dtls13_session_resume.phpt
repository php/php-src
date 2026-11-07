--TEST--
dtls:// DTLS 1.3 session resumption with the ticket the client received
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
if (!in_array('dtlsv1.3', stream_get_transports(), true)) die('skip DTLS 1.3 not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls13_session_resume.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'dtls13-resume',
    ]]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer1 = stream_socket_accept($server, 10);
    fwrite($peer1, strtoupper(fread($peer1, 64)));
    fread($peer1, 1);
    fclose($peer1);

    phpt_notify();
    $peer2 = stream_socket_accept($server, 10);
    $crypto = stream_get_meta_data($peer2)['crypto'];
    fwrite($peer2, strtoupper(fread($peer2, 64)));
    fread($peer2, 1);
    fclose($peer2);
    fclose($server);

    phpt_notify(message: $crypto['protocol'] . " reused=" . ($crypto['session_reused'] ? "yes" : "no"));
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $session = null;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function ($stream, $s) use (&$session) { $session = $s; },
    ]]);
    $c1 = stream_socket_client("dtls://{{ ADDR }}", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx);
    fwrite($c1, "ping");
    // The ticket follows the handshake: it is there once the reply was read
    var_dump(fread($c1, 64));
    $meta = stream_get_meta_data($c1)['crypto'];
    echo "captured=" . ($session instanceof Openssl\Session ? "yes" : "no") . "\n";
    echo "first_reused=" . ($meta['session_reused'] ? "yes" : "no") . "\n";
    fclose($c1);

    phpt_wait();
    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $session,
    ]]);
    $c2 = stream_socket_client("dtls://{{ ADDR }}", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctx2);
    fwrite($c2, "ping");
    $meta = stream_get_meta_data($c2)['crypto'];
    echo "second: " . $meta['protocol'] . " reused=" . ($meta['session_reused'] ? "yes" : "no") . "\n";
    fread($c2, 64);
    fclose($c2);

    echo "server: " . trim(phpt_wait()) . "\n";
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls13_session_resume.pem.tmp');
?>
--EXPECT--
string(4) "PING"
captured=yes
first_reused=no
second: DTLSv1.3 reused=yes
server: DTLSv1.3 reused=yes
