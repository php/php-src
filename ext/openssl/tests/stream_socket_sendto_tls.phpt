--TEST--
stream_socket_sendto() on a stream with TLS enabled sends the data through the TLS session
--EXTENSIONS--
openssl
--FILE--
<?php
$pem = __DIR__ . DIRECTORY_SEPARATOR . 'stream_socket_sendto_tls.pem.tmp';

include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('stream_socket_sendto_tls', $pem);

$listener = stream_socket_server('tcp://127.0.0.1:0', context: stream_context_create(['ssl' => ['local_cert' => $pem]]));
$client = stream_socket_client('tcp://' . stream_socket_get_name($listener, false), context: stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]));
$server = stream_socket_accept($listener);
stream_set_blocking($client, false);
stream_set_blocking($server, false);
do {
    $clientDone = stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
    $serverDone = stream_socket_enable_crypto($server, true, STREAM_CRYPTO_METHOD_TLS_SERVER);
} while ($clientDone === 0 || $serverDone === 0);

var_dump(fwrite($client, 'ENCRYPTED'));
var_dump(fread($server, 8192));
var_dump(stream_socket_sendto($client, 'SENT WITH SENDTO'));
var_dump(@fread($server, 8192));
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_socket_sendto_tls.pem.tmp');
?>
--EXPECT--
int(9)
string(9) "ENCRYPTED"
int(16)
string(16) "SENT WITH SENDTO"
