--TEST--
TLS over an inner stream: the inner stream carries one stream at a time and tcp:// ignores it
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_shared.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('inner-stream', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    fwrite($peer, strtoupper(fread($peer, 8192)));
    fclose($peer);
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $inner = stream_socket_client('tcp://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT);
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'inner_stream' => $inner,
    ]]);
    $first = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    var_dump($first !== false);

    // The inner stream is taken
    $second = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    var_dump($second);
    // The stream that failed to take it did not release it either
    var_dump(@fclose($inner));

    // A plain transport connects on its own
    $plain = stream_socket_client('tcp://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    var_dump(stream_socket_get_name($plain, false) !== stream_socket_get_name($inner, false));
    fclose($plain);
    var_dump(@fclose($inner));

    fwrite($first, "ping");
    var_dump(fread($first, 4));
    fclose($first);
    var_dump(fclose($inner));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_shared.pem.tmp');
?>
--EXPECTF--
bool(true)

Warning: stream_socket_client(): inner_stream is already used by another stream in %s on line %d

Warning: stream_socket_client(): Unable to connect to tls://%s in %s on line %d
bool(false)
bool(false)
bool(true)
bool(false)
string(4) "PING"
bool(true)
