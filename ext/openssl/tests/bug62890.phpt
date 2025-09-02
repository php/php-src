--TEST--
Bug #62890 (default_socket_timeout=-1 causes connection to timeout)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--INI--
default_socket_timeout=-1
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug62890.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'security_level' => 1,
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);
    @stream_socket_accept($server, 3);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'security_level' => 1,
    ]]);

    $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 3, $flags, $ctx);
    var_dump($client);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('bug62890', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug62890.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)
