--TEST--
Bug #65538: SSL context "cafile" supports stream wrappers
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => __DIR__ . '/bug54992.pem',
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
    $clientCtx = stream_context_create(['ssl' => [
        'cafile' => 'file://' . __DIR__ . '/bug54992-ca.pem',
        'peer_name' => 'bug54992.local',
    ]]);

    phpt_wait();
    $html = file_get_contents($serverUri, false, $clientCtx);

    var_dump($html);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
--EXPECT--
string(12) "Hello World!"
