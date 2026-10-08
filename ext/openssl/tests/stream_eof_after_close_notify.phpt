--TEST--
feof() is true after a TLS close_notify even if an earlier read left errno set to EAGAIN
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_eof_after_close_notify.pem.tmp';

$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $sock = stream_socket_server("tls://127.0.0.1:0", $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($sock);

    $link = stream_socket_accept($sock);
    /* Let the client block in fread() first, so its SSL_read() sees WANT_READ */
    phpt_wait();
    usleep(100000);
    fwrite($link, "data");
    /* close_notify only, the TCP connection stays open */
    stream_socket_enable_crypto($link, false);
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

    phpt_notify();
    var_dump(fread($sock, 4));
    var_dump(fread($sock, 4));
    var_dump(feof($sock));
    phpt_notify();
CODE;

include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('stream_eof_after_close_notify', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_eof_after_close_notify.pem.tmp');
?>
--EXPECT--
string(4) "data"
string(0) ""
bool(true)
