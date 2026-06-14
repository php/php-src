--TEST--
stream_socket_get_crypto_status(): reports WANT_READ/WANT_WRITE during a non-blocking handshake
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'crypto_status_handshake.pem.tmp';
$peerName = 'crypto-status-handshake';

/* Plain blocking TLS server. */
$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $server = stream_socket_server("tls://127.0.0.1:0", $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 30);
    if ($conn) {
        fwrite($conn, "ok\n");
        phpt_wait();
        fclose($conn);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

/* Client connects over plain TCP, then completes the TLS handshake in non-blocking mode, using
 *  the reported crypto status to select the right direction to wait on. */
$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'peer_name' => '%s',
    ]]);

    $client = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
    stream_set_blocking($client, false);

    $sawWant = false;
    $pendingAlwaysWant = true;

    do {
        $r = stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
        if ($r === 0) {
            $status = stream_socket_get_crypto_status($client);
            if ($status === STREAM_CRYPTO_STATUS_WANT_READ
                    || $status === STREAM_CRYPTO_STATUS_WANT_WRITE) {
                $sawWant = true;
            } else {
                /* must never be NONE while the handshake is still pending */
                $pendingAlwaysWant = false;
            }

            /* Wait on the direction the engine actually asked for. */
            $read = $write = $except = null;
            if ($status === STREAM_CRYPTO_STATUS_WANT_WRITE) {
                $write = [$client];
            } else {
                $read = [$client];
            }
            stream_select($read, $write, $except, 1);
        }
    } while ($r === 0);

    var_dump($r);
    var_dump($sawWant);
    var_dump($pendingAlwaysWant);
    /* After a completed handshake the status is reset to NONE. */
    var_dump(stream_socket_get_crypto_status($client) === STREAM_CRYPTO_STATUS_NONE);

    stream_set_blocking($client, true);
    echo trim(fgets($client)), "\n";
    phpt_notify();
    fclose($client);
CODE;
$clientCode = sprintf($clientCode, $peerName);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'crypto_status_handshake.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
ok
