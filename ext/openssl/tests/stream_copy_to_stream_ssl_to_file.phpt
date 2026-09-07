--TEST--
stream_copy_to_stream() from a TLS stream copies decrypted data (no fd fast-path)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_copy_ssl.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_copy_ssl-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);
    $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $server = stream_socket_server("ssl://127.0.0.1:0", $errno, $errstr, $flags, $serverCtx);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 5);
    fwrite($conn, str_repeat("secret-", 1000));
    fclose($conn);
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'stream_copy_ssl_peer';
$clientCode = <<<'CODE'
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => true,
        'cafile' => '%s',
        'peer_name' => '%s',
    ]]);
    $client = stream_socket_client("ssl://{{ ADDR }}", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $clientCtx);

    $tmp = tmpfile();
    /* If the copy offloaded the raw socket fd it would write ciphertext; the
     * decrypted plaintext proves it correctly fell back to the userspace loop. */
    $copied = stream_copy_to_stream($client, $tmp);
    var_dump($copied);

    fseek($tmp, 0, SEEK_SET);
    $content = stream_get_contents($tmp);
    var_dump(strlen($content));
    var_dump($content === str_repeat("secret-", 1000));

    fclose($tmp);
    fclose($client);
CODE;
$clientCode = sprintf($clientCode, $cacertFile, $peerName);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_copy_ssl.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_copy_ssl-ca.pem.tmp');
?>
--EXPECT--
int(7000)
int(7000)
bool(true)
