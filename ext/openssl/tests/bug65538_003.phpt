--TEST--
Bug #65538: SSL context "cafile" supports phar wrapper
--EXTENSIONS--
openssl
phar
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--INI--
phar.readonly=0
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug65538_003.pem.tmp';

$cacertFile = 'bug65538_003-ca.pem';
$cacertPhar = __DIR__ . DIRECTORY_SEPARATOR . 'bug65538_003-ca.phar.tmp';

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

$peerName = 'bug65538_003';
$clientCode = <<<'CODE'
    $serverUri = "https://127.0.0.1:64321/";
    $clientCtx = stream_context_create(['ssl' => [
        'cafile' => 'phar://%s/%s',
        'peer_name' => '%s',
    ]]);

    phpt_wait();
    $html = file_get_contents($serverUri, false, $clientCtx);

    var_dump($html);
CODE;
$clientCode = sprintf($clientCode, $cacertPhar, $cacertFile, $peerName);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

$phar = new Phar($cacertPhar);
$phar->addFromString($cacertFile, $certificateGenerator->getCaCert());

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug65538_003.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug65538_003-ca.phar.tmp');
?>
--EXPECT--
string(12) "Hello World!"
