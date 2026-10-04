--TEST--
TLS over an inner stream: the ciphertext goes through a tcp:// stream given by inner_stream
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('inner-stream', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    fwrite($peer, strtoupper(fread($peer, 8192)));
    fwrite($peer, str_repeat('x', 100000));
    fclose($peer);
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $inner = stream_socket_client('tcp://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT);
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'inner_stream' => $inner,
    ]]);
    // Nothing is connected here: the inner stream carries the ciphertext
    $tls = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    var_dump($tls !== false);
    var_dump(stream_get_meta_data($tls)['crypto']['protocol'] !== '');
    fwrite($tls, "ping");
    var_dump(fread($tls, 4));
    $total = 0;
    while (!feof($tls) && ($chunk = fread($tls, 8192)) !== false && $chunk !== '') {
        $total += strlen($chunk);
    }
    var_dump($total);
    // The inner stream cannot be closed from under the TLS stream
    var_dump(@fclose($inner));
    fclose($tls);
    var_dump(is_resource($inner));
    var_dump(fclose($inner));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
string(4) "PING"
int(100000)
bool(false)
bool(true)
bool(true)
