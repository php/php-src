--TEST--
Bug #69215: Crypto servers should send client CA list
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$caCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug69215-ca.pem.tmp';
$clientCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug69215-client.pem.tmp';
$serverCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug69215-server.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'cafile' => '%s',
        'verify_peer' => true,
        'verify_peer_name' => true,
        'peer_name' => 'bug69215-client',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    stream_socket_accept($server, 30);
CODE;
$serverCode = sprintf($serverCode, $serverCertFile, $caCertFile);

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'cafile' => '%s',
        'verify_peer' => true,
        'verify_peer_name' => true,
        'peer_name' => 'bug69215-server',
    ]]);

    phpt_wait();

    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));
CODE;
$clientCode = sprintf($clientCode, $clientCertFile, $caCertFile);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($caCertFile);
$certificateGenerator->saveNewCertAsFileWithKey('bug69215-client', $clientCertFile);
$certificateGenerator->saveNewCertAsFileWithKey('bug69215-server', $serverCertFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug69215-ca.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug69215-client.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug69215-server.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)
