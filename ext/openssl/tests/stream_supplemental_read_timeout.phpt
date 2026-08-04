--TEST--
Timeout for supplemental read at end of a blocking stream in SSL stream wrapper
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'crypto_supplemental_read_timeout.pem.tmp';
$peerName = 'crypto-supplemental-read-timeout';

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $server = stream_socket_server("tls://127.0.0.1:0", $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 30);

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
    stream_set_blocking($client, true);
    stream_set_timeout($client, 5);
    $start = hrtime(true);

    $buf = '';
    $read = [$client];
    $write = $except = null;
    while (true) {
        if (!stream_select($read, $write, $except, 5)) {
            break;
        }

        // Initially, read only the first char, then request more than is stored
        // in the buffer, triggering a supplemental read.
        $chunk = fread($client, strlen($buf) === 0 ? 1 : 10);
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

    $diff = (hrtime(true) - $start) / 1e9;
    var_dump($diff < 4.0);

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
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'crypto_supplemental_read_timeout.pem.tmp');
?>
--EXPECT--
hello
bool(true)
