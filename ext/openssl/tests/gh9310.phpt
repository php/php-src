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

$sniCaFile = __DIR__ . '/gh9310_sni_ca.pem.tmp';
$sniCsFile = __DIR__ . '/gh9310_sni_cs.pem.tmp';
$sniUkCertFile = __DIR__ . '/gh9310_sni_uk_cert.pem.tmp';
$sniUkKeyFile = __DIR__ . '/gh9310_sni_uk_key.pem.tmp';
$sniUsCertFile = __DIR__ . '/gh9310_sni_us_cert.pem.tmp';
$sniUsKeyFile = __DIR__ . '/gh9310_sni_us_key.pem.tmp';
$baseDirSniUkCertFile = $baseDir . '/sni_uk_cert.pem';

$certificateGenerator->saveCaCert($sniCaFile);
$certificateGenerator->saveNewCertAsFileWithKey('cs.php.net', $sniCsFile);
$certificateGenerator->saveNewCertAndKey('uk.php.net', $sniUkCertFile, $sniUkKeyFile);
$certificateGenerator->saveNewCertAndKey('us.php.net', $sniUsCertFile, $sniUsKeyFile);

copy($sniUkCertFile, $baseDirSniUkCertFile);


$serverCodeTemplate = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir',  __DIR__ . '/gh9310');
    $serverUri = "ssl://127.0.0.1:0";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'local_pk' => '%s',
    ]]);

    $sock = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify_server_start($sock);

    $link = stream_socket_accept($sock);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://{{ ADDR }}";
    $clientFlags = STREAM_CLIENT_CONNECT;

    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false
    ]]);

    @stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx);
CODE;

$sniServerCodeV1 = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir', __DIR__ . '/gh9310');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => '%s',
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    stream_socket_accept($server);
CODE;
$sniServerCodeV1 = sprintf($sniServerCodeV1, $sniCsFile);

$sniServerCodeV2 = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir', __DIR__ . '/gh9310');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "uk.php.net" => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ]
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    stream_socket_accept($server);
CODE;
$sniServerCodeV2 = sprintf($sniServerCodeV2, $baseDirSniUkCertFile, $sniUkKeyFile);

$sniServerCodeV3 = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir', __DIR__ . '/gh9310');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "us.php.net" => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ]
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    stream_socket_accept($server);
CODE;
$sniServerCodeV3 = sprintf($sniServerCodeV3, $sniUsCertFile, $sniUsKeyFile);

$sniClientCodeTemplate = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctxArr = [
        'cafile' => '%s',
        'peer_name' => '%s',
    ];

    $ctx = stream_context_create(['ssl' => $ctxArr]);
    @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 1, $flags, $ctx);
CODE;

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile . "\0test", $baseDirPkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile, $baseDirPkFile . "\0test");
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$serverCode = sprintf($serverCodeTemplate, $certFile, $pkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile, $pkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

$sniClientCode = sprintf($sniClientCodeTemplate, $sniCaFile, 'cs.php.net');
ServerClientTestCase::getInstance()->run($sniClientCode, $sniServerCodeV1);

$sniClientCode = sprintf($sniClientCodeTemplate, $sniCaFile, 'uk.php.net');
ServerClientTestCase::getInstance()->run($sniClientCode, $sniServerCodeV2);

$sniClientCode = sprintf($sniClientCodeTemplate, $sniCaFile, 'us.php.net');
ServerClientTestCase::getInstance()->run($sniClientCode, $sniServerCodeV3);

?>
--CLEAN--
<?php
$baseDir = __DIR__ . '/gh9310';

@unlink(__DIR__ . '/gh9310.crt');
@unlink(__DIR__ . '/gh9310.key');
@unlink($baseDir . '/cert.crt');
@unlink($baseDir . '/private.key');
@unlink($baseDir . '/sni_uk_cert.pem');
@unlink(__DIR__ . '/gh9310_sni_ca.pem.tmp');
@unlink(__DIR__ . '/gh9310_sni_cs.pem.tmp');
@unlink(__DIR__ . '/gh9310_sni_uk_cert.pem.tmp');
@unlink(__DIR__ . '/gh9310_sni_uk_key.pem.tmp');
@unlink(__DIR__ . '/gh9310_sni_us_cert.pem.tmp');
@unlink(__DIR__ . '/gh9310_sni_us_key.pem.tmp');
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
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%sgh9310_sni_cs.pem.tmp) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Failed setting local cert chain file `%sgh9310_sni_cs.pem.tmp'; file not found in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%sgh9310_sni_uk_key.pem.tmp) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Failed setting local private key file `%sgh9310_sni_uk_key.pem.tmp';  could not open file in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
PHP Warning:  stream_socket_accept(): open_basedir restriction in effect. File(%sgh9310_sni_us_cert.pem.tmp) is not within the allowed path(s): (%sgh9310) in %s
PHP Warning:  stream_socket_accept(): Failed setting local cert chain file `%sgh9310_sni_us_cert.pem.tmp'; could not open file in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: %s
