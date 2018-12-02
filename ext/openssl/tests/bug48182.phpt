--TEST--
Bug #48182: ssl handshake fails during asynchronous socket connection
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug48182.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug48182-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    $client = @stream_socket_accept($server, 1);

    $data = "Sending bug48182\n" . fread($client, 8192);
    fwrite($client, $data);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'bug48182';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'cafile' => '%s',
        'peer_name' => '%s'
    ]]);

    phpt_wait();
    $client = stream_socket_client($serverUri, $errno, $errstr, 10, $clientFlags, $clientCtx);

    $data = "Sending data over to SSL server in async mode with contents like Hello World\n";

    fwrite($client, $data);
    echo fread($client, 1024);
CODE;
$clientCode = sprintf($clientCode, $cacertFile, $peerName);

echo "Running bug48182\n";

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug48182.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug48182-ca.pem.tmp');
?>
--EXPECT--
Running bug48182
Sending bug48182
Sending data over to SSL server in async mode with contents like Hello World
