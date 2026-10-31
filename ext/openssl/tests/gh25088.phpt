--TEST--
GH-25088: SNI server contexts do not inherit ciphers, ALPN and DH params
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (!defined("OPENSSL_VERSION_NUMBER") || OPENSSL_VERSION_NUMBER < 0x10101000) die("skip OpenSSL 1.1.1+ required");
?>
--FILE--
<?php
$baseCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_base_cert.pem.tmp';
$baseKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_base_key.pem.tmp';
$sniCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_sni_cert.pem.tmp';
$sniKeyFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_sni_key.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAndKey('base.php.net', $baseCertFile, $baseKeyFile);
$certificateGenerator->saveNewCertAndKey('sni.php.net', $sniCertFile, $sniKeyFile);

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'local_pk' => '%s',
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
        'ciphers' => 'ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384',
        'alpn_protocols' => 'h2,http/1.1',
        'dh_param' => '%s',
        'SNI_server_certs' => [
            'sni.php.net' => [
                'local_cert' => '%s',
                'local_pk' => '%s',
            ],
        ],
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    for ($i = 0; $i < 8; $i++) {
        $conn = @stream_socket_accept($server, 3);
        if ($conn) {
            fclose($conn);
        }
    }
CODE;
$serverCode = sprintf($serverCode, $baseCertFile, $baseKeyFile, __DIR__ . DIRECTORY_SEPARATOR . 'gh25088_dh.pem', $sniCertFile, $sniKeyFile);

$clientCode = <<<'CODE'
    function connect(string $peerName, array $opts) {
        $ctx = stream_context_create(['ssl' => $opts + [
            'verify_peer' => false,
            'verify_peer_name' => false,
            'peer_name' => $peerName,
            'capture_peer_cert' => true,
            'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
        ]]);
        $client = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
        if (!$client) {
            return 'handshake failed';
        }
        $cn = openssl_x509_parse(stream_context_get_options($ctx)['ssl']['peer_certificate'])['subject']['CN'];
        $crypto = stream_get_meta_data($client)['crypto'];
        return sprintf('%s %s ALPN=%s', $cn, $crypto['cipher_name'], $crypto['alpn_protocol'] ?? 'none');
    }

    foreach (['base.php.net', 'sni.php.net'] as $peerName) {
        echo "$peerName:\n";
        echo "  disallowed cipher: ", connect($peerName, ['ciphers' => 'ECDHE-RSA-AES128-GCM-SHA256']), "\n";
        echo "  allowed cipher:    ", connect($peerName, ['ciphers' => 'ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384']), "\n";
        echo "  DHE cipher:        ", connect($peerName, ['ciphers' => 'DHE-RSA-AES256-GCM-SHA384']), "\n";
        echo "  ALPN:              ", connect($peerName, ['alpn_protocols' => 'h2']), "\n";
    }
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_base_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_base_key.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_sni_cert.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh25088_sni_key.pem.tmp');
?>
--EXPECT--
base.php.net:
  disallowed cipher: handshake failed
  allowed cipher:    base.php.net ECDHE-RSA-AES256-GCM-SHA384 ALPN=none
  DHE cipher:        base.php.net DHE-RSA-AES256-GCM-SHA384 ALPN=none
  ALPN:              base.php.net ECDHE-RSA-AES256-GCM-SHA384 ALPN=h2
sni.php.net:
  disallowed cipher: handshake failed
  allowed cipher:    sni.php.net ECDHE-RSA-AES256-GCM-SHA384 ALPN=none
  DHE cipher:        sni.php.net DHE-RSA-AES256-GCM-SHA384 ALPN=none
  ALPN:              sni.php.net ECDHE-RSA-AES256-GCM-SHA384 ALPN=h2
