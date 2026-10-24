--TEST--
Peer verification matches an IPADDR SAN when connecting to a bracketed IPv6 URI
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
$probe = @stream_socket_server("tcp://[::1]:0", $errno, $errstr);
if (!$probe) die("skip IPv6 loopback not available");
fclose($probe);
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'peer_verification_ipv6_san.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'peer_verification_ipv6_san-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://[::1]:0";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify_server_start($server);

    $client = @stream_socket_accept($server, 3);
    if ($client) {
        fwrite($client, "HELLO\n");
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $serverUri = "ssl://{{ ADDR }}";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'cafile' => '%s',
    ]]);

    $client = @stream_socket_client($serverUri, $errno, $errstr, 3, $clientFlags, $clientCtx);
    if (!$client) {
        echo "connect failed: $errstr\n";
        return;
    }
    echo trim(fread($client, 16)), "\n";
CODE;
$clientCode = sprintf($clientCode, $cacertFile);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey('ipv6-san', $certFile, null, 'IP:::1');

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'peer_verification_ipv6_san.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'peer_verification_ipv6_san-ca.pem.tmp');
?>
--EXPECT--
HELLO
