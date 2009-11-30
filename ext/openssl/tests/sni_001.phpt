--TEST--
SNI 001
--SKIPIF--
<?php
	if (!extension_loaded('openssl')) die("skip openssl extension not available");
	if (!getenv('SNI_TESTS')) die("skip Set SNI_TESTS to enable this test (uses remote resources)");
?>
--FILE--
<?php
/* Server Name Indication (SNI) tests
 * 
 * This test relies on https://sni.velox.ch/ and thus is disabled by default.
 *
 * sni.velox.ch uses 3 certificates :
 * - CN=alice.sni.velox.ch (sent in response to server_name = alice.sni.velox.ch or not set)
 * - CN=bob.sni.velox.ch (sent in response to server_name = bob.sni.velox.ch)
 * - CN=*.sni.velox.ch (sent in response to server_name = mallory.sni.velox.ch or *.sni.velox.ch or sni.velox.ch)
 *
 * The test sends requests to the server, sending different names, and checks which certificate
 * the server returned.
 */

function context() {
	return stream_context_create(array(
		'ssl' => array(
			'capture_peer_cert' => true,
		),
	));
}

function get_CN($context) {

	$ary = stream_context_get_options($context);
	assert($ary);

	$cert = $ary['ssl']['peer_certificate'];
	assert($cert);

	$cert_ary = openssl_x509_parse($cert);
	return $cert_ary['subject']['CN'];
}

function do_http_test($url, $context) {

	$fh = fopen($url, 'r', false, $context);
	assert($fh);

	var_dump(get_CN($context));
}

function do_ssl_test($url, $context) {

	$fh = stream_socket_client($url, $errno, $errstr, 
			ini_get("default_socket_timeout"), STREAM_CLIENT_CONNECT, $context);
	assert($fh);

	var_dump(get_CN($context));
}

function do_enable_crypto_test($url, $context) {

	$fh = stream_socket_client($url, $errno, $errstr,
		ini_get("default_socket_timeout"), STREAM_CLIENT_CONNECT, $context);
	assert($fh);

	$r = stream_socket_enable_crypto($fh, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
	assert($r);

	var_dump(get_CN($context));
}

/* Test https:// streams */

echo "-- auto host name (1) --\n";
do_http_test('https://alice.sni.velox.ch/', context());

echo "-- auto host name (2) --\n";
do_http_test('https://bob.sni.velox.ch/', context());

echo "-- auto host name (3) --\n";
do_http_test('https://bob.sni.velox.ch./', context());

echo "-- user supplied server name --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_server_name', 'bob.sni.velox.ch');
stream_context_set_option($context, 'http', 'header', b'Host: bob.sni.velox.ch');
do_http_test('https://alice.sni.velox.ch/', $context);

echo "-- sni disabled --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_enabled', false);
do_http_test('https://bob.sni.velox.ch/', $context);

/* Test ssl:// socket streams */

echo "-- raw SSL stream (1) --\n";
do_ssl_test('ssl://bob.sni.velox.ch:443', context());

echo "-- raw SSL stream (2) --\n";
do_ssl_test('ssl://mallory.sni.velox.ch:443', context());

echo "-- raw SSL stream with user supplied sni --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_server_name', 'bob.sni.velox.ch');

do_ssl_test('ssl://mallory.sni.velox.ch:443', $context);

echo "-- raw SSL stream with sni disabled --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_enabled', false);

do_ssl_test('ssl://mallory.sni.velox.ch:443', $context);

/* Test tcp:// socket streams with SSL enabled */

echo "-- stream_socket_enable_crypto (1) --\n";

do_enable_crypto_test('tcp://bob.sni.velox.ch:443', context());

echo "-- stream_socket_enable_crypto (2) --\n";

do_enable_crypto_test('tcp://mallory.sni.velox.ch:443', context());

echo "-- stream_socket_enable_crypto with user supplied sni --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_server_name', 'bob.sni.velox.ch');

do_enable_crypto_test('tcp://mallory.sni.velox.ch:443', $context);

echo "-- stream_socket_enable_crypto with sni disabled --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_enabled', false);

do_enable_crypto_test('tcp://mallory.sni.velox.ch:443', $context);

echo "-- stream_socket_enable_crypto with long name --\n";

$context = context();
stream_context_set_option($context, 'ssl', 'SNI_server_name', str_repeat('a.', 500) . '.sni.velox.ch');

do_enable_crypto_test('tcp://mallory.sni.velox.ch:443', $context);

?>
--EXPECTF--
-- auto host name (1) --
%unicode|string%(18) "alice.sni.velox.ch"
-- auto host name (2) --
%unicode|string%(16) "bob.sni.velox.ch"
-- auto host name (3) --
%unicode|string%(16) "bob.sni.velox.ch"
-- user supplied server name --
%unicode|string%(16) "bob.sni.velox.ch"
-- sni disabled --
%unicode|string%(18) "alice.sni.velox.ch"
-- raw SSL stream (1) --
%unicode|string%(16) "bob.sni.velox.ch"
-- raw SSL stream (2) --
%unicode|string%(14) "*.sni.velox.ch"
-- raw SSL stream with user supplied sni --
%unicode|string%(16) "bob.sni.velox.ch"
-- raw SSL stream with sni disabled --
%unicode|string%(18) "alice.sni.velox.ch"
-- stream_socket_enable_crypto (1) --
%unicode|string%(16) "bob.sni.velox.ch"
-- stream_socket_enable_crypto (2) --
%unicode|string%(14) "*.sni.velox.ch"
-- stream_socket_enable_crypto with user supplied sni --
%unicode|string%(16) "bob.sni.velox.ch"
-- stream_socket_enable_crypto with sni disabled --
%unicode|string%(18) "alice.sni.velox.ch"
-- stream_socket_enable_crypto with long name --
%unicode|string%(18) "alice.sni.velox.ch"
