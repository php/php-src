--TEST--
A stream poll handle takes the descriptor of a TLS stream without touching its buffers
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_poll_handle_cast.pem.tmp';

$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $sock = stream_socket_server("tls://127.0.0.1:0", $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($sock);

    $link = stream_socket_accept($sock);
    /* One record of 100 bytes */
    fwrite($link, str_repeat("x", 100));
    phpt_wait();
    fclose($link);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ]]);
    $sock = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 2, STREAM_CLIENT_CONNECT, $clientCtx);

    /* Unbuffered: 10 bytes leave 90 decrypted inside OpenSSL, none in the stream */
    stream_set_read_buffer($sock, 0);
    var_dump(strlen(fread($sock, 10)));

    /* Adding the handle casts for the descriptor: the select cast would move the 90
     * bytes into the stream buffer here, the poll cast leaves them where they are */
    $ctx = new Io\Poll\Context();
    $w = $ctx->add(StreamPollWeakHandle::create($sock), [Io\Poll\Event::Read]);
    var_dump(stream_get_meta_data($sock)['unread_bytes']);

    /* Bytes OpenSSL holds are not readiness of the socket */
    var_dump(count($ctx->wait(Time\Duration::fromMilliseconds(100))));
    var_dump(strlen(fread($sock, 90)));

    /* The peer's close is */
    phpt_notify();
    $fired = $ctx->wait(Time\Duration::fromSeconds(2));
    var_dump(count($fired), $fired[0] === $w);
    var_dump(fread($sock, 10), feof($sock));
CODE;

include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('stream_poll_handle_cast', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_poll_handle_cast.pem.tmp');
?>
--EXPECT--
int(10)
int(0)
int(0)
int(90)
int(1)
bool(true)
string(0) ""
bool(true)
