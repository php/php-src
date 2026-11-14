--TEST--
TLS handshake on a socket that was never connected fails instead of waiting for it
--EXTENSIONS--
openssl
sockets
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_unconnected_socket.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('unconnected', $certFile);

$stream = socket_export_stream(socket_create(AF_INET, SOCK_STREAM, SOL_TCP));
stream_context_set_option($stream, 'ssl', 'local_cert', $certFile);
stream_set_timeout($stream, 2);
var_dump(@stream_socket_enable_crypto($stream, true, STREAM_CRYPTO_METHOD_TLS_SERVER));
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_unconnected_socket.pem.tmp');
?>
--EXPECT--
bool(false)
