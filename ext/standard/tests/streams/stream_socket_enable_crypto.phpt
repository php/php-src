--TEST--
mixed stream_socket_enable_crypto(resource $stream , bool $enable [, int $crypto_type [, resource $session_stream ]] ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") { die('SKIP php version so lower.'); }
if (!extension_loaded('openssl')) { die('skip ext/openssl required'); }
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	die('skip not for windows');
}
?>
--FILE--
<?php
$serverUri = "tcp://127.0.0.1:31854";
$sock = stream_socket_server($serverUri, $errno, $errstr);

if (is_resource($sock)) {
  var_dump(stream_socket_enable_crypto($sock, false));
  var_dump(stream_socket_enable_crypto($sock, true));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv23_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv3_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv2_CLIENT));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_TLS_CLIENT));  
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv23_SERVER));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv2_SERVER));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_SSLv3_SERVER));
  var_dump(stream_socket_enable_crypto($sock, true, STREAM_CRYPTO_METHOD_TLS_SERVER));
} else {
  die("Test stream_socket_enable_crypto has failed; Unable to connect: {$errstr} ({$errno})");
}
?>
--CLEAN--
<?php
unset($serverUri);
unset($sock);
unset($errno);
unset($errstr);
?>
--EXPECTF--
bool(false)

Warning: stream_socket_enable_crypto(): When enabling encryption you must specify the crypto type in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL: Broken pipe in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): SSL/TLS already set-up for this stream in %s on line %d
bool(false)
