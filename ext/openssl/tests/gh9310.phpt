--TEST--
GH-9310: local_cert and local_pk do not respect open_basedir restriction
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
include 'ServerClientTestCase.inc';

$baseDir = __DIR__ . '/gh9310';
@mkdir($baseDir);
$baseDirCertFile = $baseDir . '/cert.crt';
$baseDirPkFile = $baseDir . '/private.key';
$certFile = __DIR__ . '/gh9310.crt';
$pkFile = __DIR__ . '/gh9310.key';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAndKey('gh9310', $certFile, $pkFile);

copy($certFile, $baseDirCertFile);
copy($pkFile, $baseDirPkFile);
copy(__DIR__ . '/sni_server_uk_cert.pem', $baseDir . '/sni_server_uk_cert.pem');


$serverCodeTemplate = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir',  __DIR__ . '/gh9310');
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'local_pk' => '%s',
    ]]);

    $sock = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    $link = stream_socket_accept($sock);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;

    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false
    ]]);

    phpt_wait();
    @stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx);
CODE;

$sniServerCodeV1 = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir', __DIR__ . '/gh9310');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => __DIR__ . "/sni_server_cs.pem",
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);
    phpt_notify();

    stream_socket_accept($server);
CODE;

$sniServerCodeV2 = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir', __DIR__ . '/gh9310');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "uk.php.net" => [
                'local_cert' => __DIR__ . '/gh9310/sni_server_uk_cert.pem',
                'local_pk' => __DIR__ . '/sni_server_uk_key.pem',
            ]
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);
    phpt_notify();

    stream_socket_accept($server);
CODE;

$sniServerCodeV3 = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir', __DIR__ . '/gh9310');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "us.php.net" => [
                'local_cert' => __DIR__ . '/sni_server_us_cert.pem',
                'local_pk' => __DIR__ . '/sni_server_us_key.pem',
            ]
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);
    phpt_notify();

    stream_socket_accept($server);
CODE;

$sniClientCodeTemplate = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctxArr = [
        'cafile' => __DIR__ . '/sni_server_ca.pem',
    ];

    phpt_wait();

    $ctxArr['peer_name'] = '%s';
    $ctx = stream_context_create(['ssl' => $ctxArr]);
    @stream_socket_client("tls://127.0.0.1:64321", $errno, $errstr, 1, $flags, $ctx);
CODE;

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile . "\0test", $baseDirPkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile, $baseDirPkFile . "\0test");
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$serverCode = sprintf($serverCodeTemplate, $certFile, $pkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile, $pkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$sniClientCode = sprintf($sniClientCodeTemplate, 'cs.php.net');
ServerClientTestCase::getInstance()->run($sniClientCode, $sniServerCodeV1);

$sniClientCode = sprintf($sniClientCodeTemplate, 'uk.php.net');
ServerClientTestCase::getInstance()->run($sniClientCode, $sniServerCodeV2);

$sniClientCode = sprintf($sniClientCodeTemplate, 'us.php.net');
ServerClientTestCase::getInstance()->run($sniClientCode, $sniServerCodeV3);

?>
--CLEAN--
<?php
$baseDir = __DIR__ . '/gh9310';

@unlink(__DIR__ . '/gh9310.crt');
@unlink(__DIR__ . '/gh9310.key');
@unlink($baseDir . '/cert.crt');
@unlink($baseDir . '/private.key');
@unlink($baseDir . '/sni_server_uk_cert.pem');
@rmdir($baseDir);
?>
--EXPECTF--
PHP Warning:  stream_socket_accept(): Path for local_cert in ssl stream context option must not contain any null bytes in %s
PHP Warning:  stream_socket_accept(): Unable to get real path of certificate file `%scert.crt' in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): Path for local_pk in ssl stream context option must not contain any null bytes in %s
PHP Warning:  stream_socket_accept(): Unable to get real path of private key file `%sprivate.key' in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%sgh9310.crt) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Unable to get real path of certificate file `%sgh9310.crt' in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%sgh9310.key) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Unable to get real path of private key file `%sgh9310.key' in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%ssni_server_cs.pem) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Failed setting local cert chain file `%ssni_server_cs.pem'; file not found in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%ssni_server_uk_key.pem) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Failed setting local private key file `%ssni_server_uk_key.pem';  could not open file in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%ssni_server_us_cert.pem) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Failed setting local cert chain file `%ssni_server_us_cert.pem'; could not open file in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
