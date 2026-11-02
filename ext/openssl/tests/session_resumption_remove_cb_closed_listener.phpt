--TEST--
TLS session resumption - session_remove_cb with a closed listener stream
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip not reliable on Windows");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_remove_cb_closed_listener.pem.tmp';

/* A TCP proxy relays the handshake, then injects a bad record so the server
 * removes the cached session after the listener has been closed. */

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'verify_peer' => false,
        'no_ticket' => true,
        'session_cache' => true,
        'session_id_context' => 'app',
        'session_new_cb'    => function($stream, $session) {},
        'session_get_cb'    => function($stream, $sessionId) { return null; },
        'session_remove_cb' => function($stream, $sessionId) {},
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    // Handshake completes during accept, so the session is cached here.
    $client = @stream_socket_accept($server, 30);

    // Free the listener while the accepted connection keeps the ctx alive.
    fclose($server);
    phpt_notify();

    // Reuse the freed listener's memory to make a dangling access fault.
    $reuse = [];
    for ($i = 0; $i < 8; $i++) {
        $reuse[] = fopen('php://memory', 'r+');
    }

    @fread($client, 1024);
    phpt_notify(message: "server alive");
    fclose($client);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$proxyCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $proxy = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, $flags);
    phpt_notify_server_start($proxy);

    $upstream = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 30, STREAM_CLIENT_CONNECT);
    $conn = stream_socket_accept($proxy, 30);
    stream_set_blocking($upstream, false);
    stream_set_blocking($conn, false);
    stream_set_blocking(STDIN, false);

    $injected = false;
    while (true) {
        $r = [$conn, $upstream, STDIN];
        $w = $e = [];
        if (@stream_select($r, $w, $e, 10) === false) {
            break;
        }
        foreach ($r as $s) {
            if ($s === STDIN) {
                if (fgets(STDIN) === false) {
                    break 2;
                }
                if (!$injected) {
                    $injected = true;
                    fwrite($upstream, "\x15\x03\x03\x00\x20" . str_repeat("\x41", 0x20));
                }
            } elseif ($s === $conn) {
                $d = @fread($conn, 8192);
                if ($d === '' || $d === false) {
                    if (feof($conn)) break 2;
                } else {
                    @fwrite($upstream, $d);
                }
            } else {
                $d = @fread($upstream, 8192);
                if ($d === '' || $d === false) {
                    if (feof($upstream)) break 2;
                } else {
                    @fwrite($conn, $d);
                }
            }
        }
    }
    fclose($conn);
    fclose($upstream);
    fclose($proxy);
CODE;

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ]]);

    $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if (!$client) {
        echo "client connect failed: $errstr\n";
        phpt_notify('proxy');
        return;
    }

    // Listener is closed; tell the proxy to inject the bad record.
    phpt_wait('server');
    phpt_notify('proxy');

    echo trim(phpt_wait('server')) . "\n";
    fclose($client);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_remove_cb_closed_listener', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, [
    'server' => $serverCode,
    'proxy'  => $proxyCode,
]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_remove_cb_closed_listener.pem.tmp');
?>
--EXPECT--
server alive
