--TEST--
dtls:// client: an empty datagram does not end the connection
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_empty_datagram.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5, $clientAddr);
    fwrite($peer, "one");
    usleep(200000);
    // A zero-length datagram from the server's address, as a spoofer could send.
    stream_socket_sendto($server, "", 0, $clientAddr);
    usleep(200000);
    fwrite($peer, "two");
    usleep(500000);
    fclose($peer);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]);
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    stream_set_timeout($client, 5);
    var_dump(fread($client, 100));
    // The empty datagram is not a record and must not end the connection.
    var_dump(fread($client, 100));
    var_dump(feof($client));
    fclose($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_empty_datagram.pem.tmp');
?>
--EXPECT--
string(3) "one"
string(3) "two"
bool(false)
