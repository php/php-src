--TEST--
Openssl\Session::__unserialize releases the previous SSL_SESSION on repeat call
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_unserialize_repeat.pem.tmp';

$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($server);

    $client = @stream_socket_accept($server, 10);
    if ($client) {
        fwrite($client, "ok\n");
        fclose($client);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $captured = null;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function ($s, $session) use (&$captured) {
            $captured = $session;
            return true;
        },
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
    ]]);

    $c = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 10,
        STREAM_CLIENT_CONNECT, $ctx);
    if (!$c) {
        echo "connect failed: $errstr\n";
        return;
    }
    fread($c, 8);
    fclose($c);

    if (!$captured instanceof Openssl\Session) {
        echo "no session captured\n";
        return;
    }

    $payload = $captured->__serialize();
    $sess = unserialize(serialize($captured));
    echo "first: " . (is_object($sess) ? get_class($sess) : "fail") . "\n";

    try {
        $sess->__unserialize($payload);
        echo "second: no throw\n";
    } catch (Error $e) {
        echo "second: " . $e->getMessage() . "\n";
    }

    echo "alive\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session-unserialize-repeat', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_unserialize_repeat.pem.tmp');
?>
--EXPECTF--
first: Openssl\Session
second: Cannot modify readonly property Openssl\Session::$id
alive
