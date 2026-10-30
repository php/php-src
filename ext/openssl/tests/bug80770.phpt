--TEST--
Bug #80770: SNI_server_certs does not inherit peer verification options
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (OPENSSL_VERSION_NUMBER < 0x10101000) die("skip OpenSSL v1.1.1 required");
?>
--FILE--
<?php
$clientCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug80770_client.pem.tmp';
$caCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug80770_ca.pem.tmp';
$csFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug80770_cs.pem.tmp';
$ukFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug80770_uk.pem.tmp';
$usFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug80770_us.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($caCertFile);
$certificateGenerator->saveNewCertAsFileWithKey('cs.php.net', $csFile);
$certificateGenerator->saveNewCertAsFileWithKey('uk.php.net', $ukFile);
$certificateGenerator->saveNewCertAsFileWithKey('us.php.net', $usFile);
$certificateGenerator->saveNewCertAsFileWithKey('Bug80770 Test Client', $clientCertFile);

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => '%s',
            "uk.php.net" => '%s',
            "us.php.net" => '%s',
        ],
        'verify_peer' => true,
        'cafile' => '%s',
        'capture_peer_cert' => true,
        'verify_peer_name' => false,
        'security_level' => 0,
    ]]);
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);
    $client = stream_socket_accept($server, 30);
    if ($client) {
        $success = stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_SERVER);
        if ($success) {
            $options = stream_context_get_options($client);
            $hasCert = isset($options['ssl']['peer_certificate']);
            phpt_notify(message: $hasCert ? "CLIENT_CERT_CAPTURED" : "NO_CLIENT_CERT");
        } else {
            phpt_notify(message: "TLS_HANDSHAKE_FAILED");
        }
    } else {
        phpt_notify(message: "ACCEPT_FAILED");
    }
CODE;
$serverCode = sprintf($serverCode, $csFile, $ukFile, $usFile, $caCertFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'local_cert' => '%s',
        'peer_name' => 'cs.php.net',
        'security_level' => 0,
    ]]);
    $client = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if ($client) {
        stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
    }
    $result = phpt_wait();
    echo trim($result);
CODE;
$clientCode = sprintf($clientCode, $clientCertFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug80770_client.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug80770_ca.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug80770_cs.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug80770_uk.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug80770_us.pem.tmp');
?>
--EXPECTF--
CLIENT_CERT_CAPTURED
