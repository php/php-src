--TEST--
GH-10495: feof hangs indefinitely
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh10495.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh10495-ca.pem.tmp';

$peerName = 'gh10495';
$clientCode = <<<'CODE'
    $context = stream_context_create(['ssl' => ['verify_peer' => false, 'peer_name' => '%s']]);

    phpt_wait('server');
    phpt_notify('proxy');

    phpt_wait('proxy');
    $fp = stream_socket_client("tlsv1.2://127.0.0.1:10012", $errornum, $errorstr, 1, STREAM_CLIENT_CONNECT, $context);

    phpt_wait('proxy');

    $time = microtime(true);
    var_dump(feof($fp));
    var_dump(microtime(true) - $time < 0.5);

    var_dump(stream_get_contents($fp, 6));

    phpt_notify('server');
    phpt_notify('proxy');
CODE;
$clientCode = sprintf($clientCode, $peerName);

$serverCode = <<<'CODE'
    $context = stream_context_create(['ssl' => ['local_cert' => '%s']]);

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $fp = stream_socket_server("tlsv1.2://127.0.0.1:10011", $errornum, $errorstr, $flags, $context);
    phpt_notify();

    $conn = stream_socket_accept($fp);
    fwrite($conn, 'warmup');

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
    $applicationData = false;
    while (stream_select($read, $write, $except, 1)) {
        foreach ($read as $fp) {
            $data = stream_get_contents($fp);
            if ($fp === $conn) {
                fwrite($upstream, $data);
            } else {
                foreach (phpt_extract_tls_records($data) as $record) {
                    if ($record !== '' && $record[0] === chr(23)) {
                        if (!$applicationData) {
                            $applicationData = true;
                            fwrite($conn, $record[0]);
                            phpt_notify();
                            sleep(1);
                            fwrite($conn, substr($record, 1));
                        } else {
                            fwrite($conn, $record);
                        }
                    } else {
                        fwrite($conn, $record);
                    }
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
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh10495.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh10495-ca.pem.tmp');
?>
--EXPECT--
bool(false)
bool(true)
string(6) "warmup"
