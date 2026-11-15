--TEST--
Server ALPN selection on a stream seeded from a closed session stream
--EXTENSIONS--
openssl
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_alpn_closed_parent.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('localhost', $certFile);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$parentClient = stream_socket_client("tcp://$addr");
$parent = stream_socket_accept($server);
stream_context_set_options($parent, ['ssl' => [
    'local_cert' => $certFile,
    'alpn_protocols' => 'h2,http/1.1',
]]);
stream_set_blocking($parent, false);
var_dump(stream_socket_enable_crypto($parent, true, STREAM_CRYPTO_METHOD_TLS_SERVER));

$client = stream_socket_client("tcp://$addr", $errno, $errstr, 5, STREAM_CLIENT_CONNECT,
    stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'alpn_protocols' => 'h2',
    ]]));
$child = stream_socket_accept($server);
stream_set_blocking($child, false);
var_dump(stream_socket_enable_crypto($child, true, STREAM_CRYPTO_METHOD_TLS_SERVER, $parent));

fclose($parent);
fclose($parentClient);

stream_set_blocking($client, false);
$clientDone = $childDone = 0;
for ($i = 0; $i < 1000 && ($clientDone !== true || $childDone !== true); $i++) {
    if ($clientDone !== true) {
        $clientDone = stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
    }
    if ($childDone !== true) {
        $childDone = stream_socket_enable_crypto($child, true, STREAM_CRYPTO_METHOD_TLS_SERVER);
    }
    usleep(1000);
}
var_dump($clientDone, $childDone);
var_dump(stream_get_meta_data($client)['crypto']['alpn_protocol']);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server_alpn_closed_parent.pem.tmp');
?>
--EXPECT--
int(0)
int(0)
bool(true)
bool(true)
string(2) "h2"
