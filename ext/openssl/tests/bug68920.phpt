--TEST--
Bug #68920: peer_fingerprint input checks should be strict
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
--FILE--
<?php
error_reporting(E_ALL);

$ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => true]]);
$sock = stream_socket_client("ssl://php.net:443", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
var_dump($sock);

$ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => null]]);
$sock = stream_socket_client("ssl://php.net:443", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
var_dump($sock);

$ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => []]]);
$sock = stream_socket_client("ssl://php.net:443", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
var_dump($sock);

$ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => ['foo']]]);
$sock = stream_socket_client("ssl://php.net:443", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
var_dump($sock);
--EXPECTF--

Warning: stream_socket_client(): Expected peer fingerprint must be a string or an array in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to %s (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Expected peer fingerprint must be a string or an array in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to %s (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Invalid peer_fingerprint array; [algo => fingerprint] form required in %s on line %d

Warning: stream_socket_client(): peer_fingerprint match failure in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to %s (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Invalid peer_fingerprint array; [algo => fingerprint] form required in %s on line %d

Warning: stream_socket_client(): peer_fingerprint match failure in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to %s (Unknown error) in %s on line %d
bool(false)
