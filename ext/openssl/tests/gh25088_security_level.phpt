--TEST--
GH-25088: SNI server contexts inherit the security level
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$baseCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_base_cert.pem.tmp';
$baseKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_base_key.pem.tmp';
$sniCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_sni_cert.pem.tmp';
$sniKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_sni_key.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAndKey('base.php.net', $baseCertFile, $baseKeyFile);
// A 768-bit key is refused at security level 1 and above.
$certificateGenerator->saveNewCertAndKey('sni.php.net', $sniCertFile, $sniKeyFile, 768);

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'local_pk' => '%s',
        'security_level' => 0,
        'SNI_server_certs' => [
            'sni.php.net' => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ],
        ],
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    for ($i = 0; $i < 2; $i++) {
        $conn = @stream_socket_accept($server, 3);
        if ($conn) {
            fclose($conn);
        }
    }
CODE;
$serverCode = sprintf($serverCode, $baseCertFile, $baseKeyFile, $sniCertFile, $sniKeyFile);

$clientCode = <<<'CODE'
    foreach (['base.php.net', 'sni.php.net'] as $peerName) {
        $ctx = stream_context_create(['ssl' => [
            'verify_peer' => false,
            'verify_peer_name' => false,
            'peer_name' => $peerName,
            'capture_peer_cert' => true,
            'security_level' => 0,
        ]]);
        $client = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
        if (!$client) {
            echo "$peerName: handshake failed\n";
            continue;
        }
        $cert = openssl_x509_parse(stream_context_get_options($ctx)['ssl']['peer_certificate']);
        echo "$peerName: ", $cert['subject']['CN'], "\n";
    }
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_base_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_base_key.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_sni_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_security_level_sni_key.pem.tmp');
?>
--EXPECT--
base.php.net: base.php.net
sni.php.net: sni.php.net
