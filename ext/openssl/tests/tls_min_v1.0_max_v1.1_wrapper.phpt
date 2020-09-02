--TEST--
tls stream wrapper with min version 1.0 and max version 1.1
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_min_v1.0_max_v1.1_wrapper.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'min_proto_version' => STREAM_CRYPTO_PROTO_TLSv1_0,
        'max_proto_version' => STREAM_CRYPTO_PROTO_TLSv1_1,
        'security_level' => 1,
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);
    phpt_notify();

    for ($i=0; $i < (phpt_has_sslv3() ? 6 : 5); $i++) {
        @stream_socket_accept($server, 3);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'security_level' => 1,
    ]]);

    phpt_wait();

    $client = stream_socket_client("tlsv1.0://127.0.0.1:64321", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);

    $client = @stream_socket_client("sslv3://127.0.0.1:64321", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);

    $client = @stream_socket_client("tlsv1.1://127.0.0.1:64321", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);

    $client = @stream_socket_client("tlsv1.2://127.0.0.1:64321", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);

    $client = @stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);

    $client = @stream_socket_client("tls://127.0.0.1:64321", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('tls_min_v1.0_max_v1.1_wrapper', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_min_v1.0_max_v1.1_wrapper.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)
bool(false)
resource(%d) of type (stream)
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)
