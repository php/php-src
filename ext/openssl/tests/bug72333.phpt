--TEST--
Bug #72333: fwrite() on non-blocking SSL sockets doesn't work
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug72333.pem.tmp';

$serverCode = <<<'CODE'
    $context = stream_context_create(['ssl' => ['local_cert' => '%s']]);

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $fp = stream_socket_server("ssl://127.0.0.1:10011", $errornum, $errorstr, $flags, $context);
    phpt_notify();
    $conn = stream_socket_accept($fp);
    $total = 100000;
    $result = fread($conn, $total);
    stream_set_blocking($conn, false);
    usleep(50000);
    $read = [$conn];
    while (stream_select($read, $write, $except, 1)) {
        $result = fread($conn, 100000);
        if (!$result) {
            break;
        }
        $total += strlen($result);
        if ($total >= 4000000) {
            break;
        }
    }
    phpt_wait();
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'bug72333';
$clientCode = <<<'CODE'
    $context = stream_context_create(['ssl' => ['verify_peer' => false, 'peer_name' => '%s']]);

    phpt_wait();
    $fp = stream_socket_client("ssl://127.0.0.1:10011", $errornum, $errorstr, 3000, STREAM_CLIENT_CONNECT, $context);
    stream_set_blocking($fp, false);

    function blocking_fwrite($fp, $buf) {
        $write = [$fp];
        $total = 0;
        while (stream_select($read, $write, $except, 1)) {
            $result = fwrite($fp, $buf);
            if (!$result) {
                break;
            }
            $total += $result;
            if ($total >= strlen($buf)) {
                return $total;
            }
            $buf = substr($buf, $total);
        }
    }
    $str1 = str_repeat("a", 4000000);
    blocking_fwrite($fp, $str1);
    phpt_notify();
    echo "done";
CODE;
$clientCode = sprintf($clientCode, $peerName);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug72333.pem.tmp');
?>
--EXPECT--
done
