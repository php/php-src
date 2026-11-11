--TEST--
Server SNI certificate selection on a stream seeded from a session stream
--EXTENSIONS--
openssl
--FILE--
<?php
$csFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_sni_cs.pem.tmp';
$ukFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_sni_uk.pem.tmp';
$usFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_sni_us.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('cs.php.net', $csFile);
$certificateGenerator->saveNewCertAsFileWithKey('uk.php.net', $ukFile);
$certificateGenerator->saveNewCertAsFileWithKey('us.php.net', $usFile);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

function handshake(string $addr, $server, string $peerName, $session, array $serverOptions = []): array
{
    $client = stream_socket_client("tcp://$addr", $errno, $errstr, 5, STREAM_CLIENT_CONNECT,
        stream_context_create(['ssl' => [
            'verify_peer' => false,
            'verify_peer_name' => false,
            'capture_peer_cert' => true,
            'peer_name' => $peerName,
        ]]));
    $accepted = stream_socket_accept($server);
    if ($serverOptions) {
        stream_context_set_options($accepted, ['ssl' => $serverOptions]);
    }
    stream_set_blocking($client, false);
    stream_set_blocking($accepted, false);

    $clientDone = $serverDone = 0;
    for ($i = 0; $i < 1000 && ($clientDone !== true || $serverDone !== true); $i++) {
        if ($clientDone !== true) {
            $clientDone = stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
        }
        if ($serverDone !== true) {
            $serverDone = stream_socket_enable_crypto($accepted, true, STREAM_CRYPTO_METHOD_TLS_SERVER, $session);
        }
        usleep(1000);
    }

    $cert = stream_context_get_options($client)['ssl']['peer_certificate'];
    var_dump(openssl_x509_parse($cert)['subject']['CN']);

    return [$accepted, $client];
}

$parent = handshake($addr, $server, 'uk.php.net', null, [
    'local_cert' => $csFile,
    'SNI_server_certs' => [
        'cs.php.net' => $csFile,
        'uk.php.net' => $ukFile,
        'us.php.net' => $usFile,
    ],
]);
$child = handshake($addr, $server, 'us.php.net', $parent[0]);

fclose($parent[0]);
fclose($parent[1]);
unset($parent);

$grandchild = handshake($addr, $server, 'uk.php.net', $child[0]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_sni_cs.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_sni_uk.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_sni_us.pem.tmp');
?>
--EXPECT--
string(10) "uk.php.net"
string(10) "us.php.net"
string(10) "uk.php.net"
