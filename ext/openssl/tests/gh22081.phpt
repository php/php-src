--TEST--
GH-22081: server reneg limit not reallocated across non-blocking retries
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh22081-server-reneg-nonblocking.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    /* Plain TCP listener so the TLS handshake is driven manually below. */
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Complete each handshake in non-blocking mode so that php_openssl_enable_crypto() is
     * re-entered across multiple WANT_READ/WANT_WRITE rounds per connection. */
    for ($i = 0; $i < 5; $i++) {
        $conn = @stream_socket_accept($server, 30);
        if (!$conn) {
            continue;
        }
        stream_set_blocking($conn, false);
        do {
            $r = stream_socket_enable_crypto($conn, true, STREAM_CRYPTO_METHOD_TLS_SERVER);
        } while ($r === 0);

        if ($r === true) {
            fwrite($conn, "ok $i\n");
        }
        fclose($conn);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ]]);

    for ($i = 0; $i < 5; $i++) {
        $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
        if ($client) {
            echo trim(fgets($client)) . "\n";
            fclose($client);
        }
    }
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('gh22081-server-reneg-nonblocking', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh22081-server-reneg-nonblocking.pem.tmp');
?>
--EXPECT--
ok 0
ok 1
ok 2
ok 3
ok 4

