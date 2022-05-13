--TEST--
Bug #76705: feof might hang on TLS streams in case of fragmented TLS records
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug77390.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug77390-ca.pem.tmp';

$peerName = 'bug77390';
$clientCode = <<<'CODE'
    $context = stream_context_create(['ssl' => ['verify_peer' => false, 'peer_name' => '%s']]);

    phpt_wait('server');
    phpt_notify('proxy');

    phpt_wait('proxy');
    $fp = stream_socket_client("ssl://127.0.0.1:10012", $errornum, $errorstr, 3000, STREAM_CLIENT_CONNECT, $context);
    stream_set_blocking($fp, false);

    $read = [$fp];
    $buf = '';
    $emptyChunkPrinted = false;
    $warmedUp = false;
    while (stream_select($read, $write, $except, 1000)) {
        $chunk = stream_get_contents($fp, 4096);
        $buf .= $chunk;
        phpt_notify('proxy');
        if (!$warmedUp) {
            if ($buf !== 'warmup') {
                continue;
            }
            $warmedUp = true;
            $buf = '';
            phpt_notify('server');
            continue;
        }
        if ($chunk !== '' || !$emptyChunkPrinted) {
            $emptyChunkPrinted = true;
            var_dump($chunk);
        }
        if ($buf === 'hello, world') {
            break;
        }
    }

    phpt_notify('server');
    phpt_notify('proxy');
CODE;
$clientCode = sprintf($clientCode, $peerName);

$serverCode = <<<'CODE'
    $context = stream_context_create(['ssl' => ['local_cert' => '%s']]);

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $fp = stream_socket_server("ssl://127.0.0.1:10011", $errornum, $errorstr, $flags, $context);
    phpt_notify();

    $conn = stream_socket_accept($fp);
    fwrite($conn, 'warmup');
    phpt_wait();
    fwrite($conn, 'hello, world');

    phpt_wait();
    fclose($conn);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$proxyCode = <<<'CODE'
    phpt_wait();

    $upstream = stream_socket_client("tcp://127.0.0.1:10011", $errornum, $errorstr, 3000, STREAM_CLIENT_CONNECT);
    stream_set_blocking($upstream, false);

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $server = stream_socket_server("tcp://127.0.0.1:10012", $errornum, $errorstr, $flags);
    phpt_notify();

    $conn = stream_socket_accept($server);
    stream_set_blocking($conn, false);

    $read = [$upstream, $conn];
    while (stream_select($read, $write, $except, 1)) {
        foreach ($read as $fp) {
            $data = stream_get_contents($fp);
            if ($fp === $conn) {
                fwrite($upstream, $data);
            } else {
                if ($data !== '' && $data[0] === chr(23)) {
                    $parts = str_split($data, (int) ceil(strlen($data) / 3));
                    foreach ($parts as $part) {
                        fwrite($conn, $part);
                        phpt_wait(null, 1);
                    }
                } else {
                    fwrite($conn, $data);
                }
            }
        }
        if (feof($upstream)) {
            break;
        }
        $read = [$upstream, $conn];
    }

    phpt_wait();
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, [
    'server' => $serverCode,
    'proxy' => $proxyCode,
]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug77390.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug77390-ca.pem.tmp');
?>
--EXPECT--
string(0) ""
string(12) "hello, world"
