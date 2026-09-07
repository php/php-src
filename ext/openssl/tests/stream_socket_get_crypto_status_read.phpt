--TEST--
stream_socket_get_crypto_status(): reports WANT_READ on a non-blocking read with no application data
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'crypto_status_read.pem.tmp';
$peerName = 'crypto-status-read';

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $server = stream_socket_server("tls://127.0.0.1:0", $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 30);

    /* Do not send anything until the client has performed its first read, so that the read is
     * guaranteed to find no application data. */
    phpt_wait();
    fwrite($conn, "hello\n");

    phpt_wait();
    fclose($conn);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'peer_name' => '%s',
    ]]);

    $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
    stream_set_blocking($client, false);

    /* No application data has been sent yet - a non-blocking read returns nothing and the crypto
     * status reflects that the OpenSSL wants to read. */
    $data = fread($client, 100);
    var_dump($data === '' || $data === false);
    var_dump(stream_socket_get_crypto_status($client) === STREAM_CRYPTO_STATUS_WANT_READ);

    /* Now let the server send. */
    phpt_notify();

    $buf = '';
    $read = [$client];
    $write = $except = null;
    while (stream_select($read, $write, $except, 5)) {
        $chunk = fread($client, 100);
        if ($chunk === '' || $chunk === false) {
            /* A non-application record (e.g. a TLS 1.3 session ticket) may arrive first. */
            if (feof($client)) {
                break;
            }
        } else {
            $buf .= $chunk;
            if (strlen($buf) >= 6) {
                break;
            }
        }
        $read = [$client];
        $write = $except = null;
    }

    echo trim($buf), "\n";
    /* A successful read clears the pending status back to NONE. */
    var_dump(stream_socket_get_crypto_status($client) === STREAM_CRYPTO_STATUS_NONE);

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
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'crypto_status_read.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
hello
bool(true)
