--TEST--
sni_server with separate pk and cert
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$caFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_ca_kc.pem.tmp';
$csCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_cs_cert.pem.tmp';
$csKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_cs_key.pem.tmp';
$ukCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_uk_cert.pem.tmp';
$ukKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_uk_key.pem.tmp';
$usCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_us_cert.pem.tmp';
$usKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'sni_server_us_key.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($caFile);
$certificateGenerator->saveNewCertAndKey('cs.php.net', $csCertFile, $csKeyFile);
$certificateGenerator->saveNewCertAndKey('uk.php.net', $ukCertFile, $ukKeyFile);
$certificateGenerator->saveNewCertAndKey('us.php.net', $usCertFile, $usKeyFile);

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ],
            "uk.php.net" => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ],
            "us.php.net" => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ],
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    for ($i=0; $i < 3; $i++) {
        @stream_socket_accept($server, 3);
    }
CODE;
$serverCode = sprintf($serverCode,
        $csCertFile, $csKeyFile,
        $ukCertFile, $ukKeyFile,
        $usCertFile, $usKeyFile
);

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
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_ca_kc.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_cs_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_cs_key.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_uk_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_uk_key.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_us_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'sni_server_us_key.pem.tmp');
?>
--EXPECTF--
string(%d) "cs.php.net"
string(%d) "uk.php.net"
string(%d) "us.php.net"
