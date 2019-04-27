--TEST--
Specific crypto method for ssl:// transports.
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => __DIR__ . '/streams_crypto_method.pem',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    $client = @stream_socket_accept($server);
    if ($client) {
        $in = '';
        while (!preg_match('/\r?\n\r?\n/', $in)) {
            $in .= fread($client, 2048);
        }
        $response = "HTTP/1.0 200 OK\r\n"
                  . "Content-Type: text/plain\r\n"
                  . "Content-Length: 12\r\n"
                  . "Connection: close\r\n"
                  . "\r\n"
                  . "Hello World!";
        fwrite($client, $response);
        fclose($client);
    }
CODE;

$clientCode = <<<'CODE'
    $serverUri = "https://127.0.0.1:64321/";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
        'verify_peer' => false,
        'verify_peer_name' => false
    ]]);

    phpt_wait();
    echo file_get_contents($serverUri, false, $clientCtx);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
Hello World!
