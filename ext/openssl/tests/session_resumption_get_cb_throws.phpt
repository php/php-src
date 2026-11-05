--TEST--
TLS session resumption - exception thrown by session_get_cb propagates unchanged
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_get_cb_throws.pem.tmp';

$serverCode = <<<'CODE'
    $sessionStore = [];

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'test-server',
        'session_new_cb' => function ($stream, $session) use (&$sessionStore) {
            $sessionStore[bin2hex($session->id)] = $session;
        },
        'session_get_cb' => function ($stream, $sessionId) use (&$sessionStore) {
            if (isset($sessionStore[bin2hex($sessionId)])) {
                throw new RuntimeException('session_get_cb failed');
            }
            return null;
        },
    ], 'stream' => ['error_mode' => StreamErrorMode::%s]]);

    $server = stream_socket_server('tlsv1.2://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    $client = @stream_socket_accept($server, 30);
    fwrite($client, "Response 1\n");
    fclose($client);

    try {
        $client = @stream_socket_accept($server, 30);
        $result = "accepted " . get_debug_type($client);
    } catch (Throwable $e) {
        $result = $e::class . ": " . $e->getMessage() . ", previous: " . get_debug_type($e->getPrevious());
    }

    phpt_notify(message: $result);
CODE;

$clientCode = <<<'CODE'
    $sessionData = null;

    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function ($stream, $session) use (&$sessionData) {
            $sessionData = $session;
        },
    ]]);

    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    echo trim(fgets($client1)), "\n";
    fclose($client1);

    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
    ]]);

    $client2 = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx2);
    var_dump($client2);

    echo "Server: ", trim(phpt_wait()), "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_get_cb_throws', $certFile);

include 'ServerClientTestCase.inc';
foreach (['Error', 'Exception'] as $mode) {
    echo "$mode mode:\n";
    ServerClientTestCase::getInstance()->run($clientCode, sprintf($serverCode, $certFile, $mode));
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_get_cb_throws.pem.tmp');
?>
--EXPECTF--
Error mode:
Response 1
resource(%d) of type (stream)
Server: RuntimeException: session_get_cb failed, previous: null
Exception mode:
Response 1
resource(%d) of type (stream)
Server: RuntimeException: session_get_cb failed, previous: null
