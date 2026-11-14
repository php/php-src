--TEST--
A session callback cannot close the stream whose handshake or read it runs in
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_callback_close.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('callback-close', $certFile);

// The callback of a dtls:// server runs inside the handshake of a peer that has no stream yet
// and gets the listener. The callback of a tls:// client runs inside its read.
$serverCode = <<<'CODE'
    $closed = [];
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'callback-close',
        'session_get_cb' => fn() => null,
        'session_new_cb' => function ($stream) use (&$closed) {
            $closed[] = @fclose($stream);
            $closed[] = @stream_socket_accept($stream, 0);
        },
    ]]);
    $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $dtls = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($dtls);

    $peer = stream_socket_accept($dtls, 10);
    fwrite($peer, strtoupper(fread($peer, 64)));
    phpt_notify(message: json_encode($closed));

    $tls = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify(message: stream_socket_get_name($tls, false));
    $closed = [];
    $peer = stream_socket_accept($tls, 10);
    fwrite($peer, strtoupper(fread($peer, 64)));
    fread($peer, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ssl = ['verify_peer' => false, 'verify_peer_name' => false];
    $c = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 10, STREAM_CLIENT_CONNECT,
        stream_context_create(['ssl' => $ssl]));
    fwrite($c, "ping");
    var_dump(fread($c, 64));
    echo "server: ", trim(phpt_wait()), "\n";
    fclose($c);

    $calls = 0;
    $addr = trim(phpt_wait());
    $c = stream_socket_client("tls://$addr", $errno, $errstr, 10, STREAM_CLIENT_CONNECT,
        stream_context_create(['ssl' => $ssl + [
            'session_new_cb' => function ($stream) use (&$calls) {
                if ($calls++ == 0) {
                    var_dump(fclose($stream));
                    var_dump(fwrite($stream, "nested"));
                }
            },
        ]]));
    fwrite($c, "ping");
    var_dump(fread($c, 64));
    var_dump($calls > 0);
    var_dump(fclose($c));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_callback_close.pem.tmp');
?>
--EXPECTF--
string(4) "PING"
server: [false,false%S]

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(false)

Warning: fwrite(): SSL: the stream is used from within its own operation in %s on line %d
int(0)
string(4) "PING"
bool(true)
bool(true)
