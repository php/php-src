--TEST--
Stream notification callback cannot close the TLS stream it is dispatched from
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'https_notification_fclose_stream.pem.tmp';

$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server("tls://127.0.0.1:0", $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($server);

    /* Send the body only after the client write. */
    $conn = stream_socket_accept($server);
    while (($line = fgets($conn)) !== false && $line !== "\r\n");
    fwrite($conn, "HTTP/1.1 200 OK\r\nContent-Length: 4\r\nConnection: close\r\n\r\n");
    fread($conn, 2);
    fwrite($conn, "body");
    fclose($conn);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $stream = null;
    $clientCtx = stream_context_create(
        ['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]],
        ['notification' => function (int $code) use (&$stream) {
            if (!is_resource($stream)) {
                return;
            }
            if ($code === STREAM_NOTIFY_PROGRESS || $code === STREAM_NOTIFY_COMPLETED) {
                echo $code === STREAM_NOTIFY_PROGRESS ? "progress\n" : "completed\n";
                var_dump(fclose($stream));
            }
        }]
    );

    $stream = fopen("https://{{ ADDR }}/", 'r', false, $clientCtx);
    var_dump(fwrite($stream, "go"));
    var_dump(fread($stream, 8192));
    var_dump(fread($stream, 8192));
    var_dump(fclose($stream));
CODE;

include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('https_notification_fclose_stream', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'https_notification_fclose_stream.pem.tmp');
?>
--EXPECTF--
progress

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
int(2)
progress

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
string(4) "body"
completed

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
string(0) ""
bool(true)
