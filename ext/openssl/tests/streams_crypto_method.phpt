--TEST--
Specific crypto method for ssl:// transports.
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'streams_crypto_method.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
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
$serverCode = sprintf($serverCode, $certFile);

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

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('streams_crypto_method', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'streams_crypto_method.pem.tmp');
?>
--EXPECT--
Hello World!
