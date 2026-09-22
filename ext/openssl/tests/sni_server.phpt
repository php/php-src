--TEST--
sni_server
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$caFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_ca.pem.tmp';
$csFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_cs.pem.tmp';
$ukFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_uk.pem.tmp';
$usFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_us.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($caFile);
$certificateGenerator->saveNewCertAsFileWithKey('cs.php.net', $csFile);
$certificateGenerator->saveNewCertAsFileWithKey('uk.php.net', $ukFile);
$certificateGenerator->saveNewCertAsFileWithKey('us.php.net', $usFile);

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => '%s',
            "uk.php.net" => '%s',
            "us.php.net" => '%s',
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    for ($i=0; $i < 3; $i++) {
        @stream_socket_accept($server, 3);
    }
CODE;
$serverCode = sprintf($serverCode, $csFile, $ukFile, $usFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctxArr = [
        'cafile' => '%s',
        'capture_peer_cert' => true
    ];

    $ctxArr['peer_name'] = 'cs.php.net';
    $ctx = stream_context_create(['ssl' => $ctxArr]);
    $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 1, $flags, $ctx);
    $cert = stream_context_get_options($ctx)['ssl']['peer_certificate'];
    var_dump(openssl_x509_parse($cert)['subject']['CN']);

    $ctxArr['peer_name'] = 'uk.php.net';
    $ctx = stream_context_create(['ssl' => $ctxArr]);
    $client = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 1, $flags, $ctx);
    $cert = stream_context_get_options($ctx)['ssl']['peer_certificate'];
    var_dump(openssl_x509_parse($cert)['subject']['CN']);

    $ctxArr['peer_name'] = 'us.php.net';
    $ctx = stream_context_create(['ssl' => $ctxArr]);
    $client = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 1, $flags, $ctx);
    $cert = stream_context_get_options($ctx)['ssl']['peer_certificate'];
    var_dump(openssl_x509_parse($cert)['subject']['CN']);
CODE;
$clientCode = sprintf($clientCode, $caFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_ca.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_cs.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_uk.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_us.pem.tmp');
?>
--EXPECTF--
string(%d) "cs.php.net"
string(%d) "uk.php.net"
string(%d) "us.php.net"
