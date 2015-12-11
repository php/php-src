--TEST--
mixed stream_socket_enable_crypto ( resource $stream , bool $enable [, int $crypto_type [, resource $session_stream ]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
  die('SKIP php version so lower.');
}
?>
--FILE--
<?php
$serverUri = "ssl://127.0.0.1:64321";
$clientFlags = STREAM_CLIENT_CONNECT;
$ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => true]]);
$sock = stream_socket_client($serverUri, $errno, $errstr, 30, $clientFlags, $ctx);

if (is_resource($db_link)) {
  var_dump(stream_socket_enable_crypto($sock, false);
  var_dump(stream_socket_enable_crypto($sock, true);
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv2_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv3_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv23_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_TLS_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv2_SERVER));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv3_SERVER));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv23_SERVER));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_TLS_SERVER));
} else {
  die("Test stream_socket_enable_crypto has failed; Unable to connect: {$errstr} ({$errno})");
}
?>
--EXPECT--
