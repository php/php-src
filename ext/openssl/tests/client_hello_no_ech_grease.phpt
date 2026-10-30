--TEST--
TLS client does not send an ECH GREASE extension it did not ask for
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x40000000) die('skip For OpenSSL >= 4.0');
?>
--FILE--
<?php
/* SSL_OP_ALL has bit 31 set; storing the context options in a signed 32-bit
 * int sign-extended it into SSL_OP_ECH_GREASE (bit 37) on OpenSSL >= 4.0. */
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$address = stream_socket_get_name($server, false);

$context = stream_context_create(['ssl' => ['verify_peer' => false]]);
$client = stream_socket_client("tcp://$address", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $context);
stream_set_blocking($client, false);
/* Sends the ClientHello, then returns 0 waiting for the ServerHello */
var_dump(stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT));

$conn = stream_socket_accept($server, 1);
$record = '';
while (strlen($record) < 5 || strlen($record) < 5 + unpack('n', $record, 3)[1]) {
    $chunk = fread($conn, 16384);
    if ($chunk === '' || $chunk === false) {
        break;
    }
    $record .= $chunk;
}

/* Skip record header (5), handshake header (4), version (2) and random (32) */
$pos = 43;
$pos += 1 + ord($record[$pos]);                  /* session id */
$pos += 2 + unpack('n', $record, $pos)[1];      /* cipher suites */
$pos += 1 + ord($record[$pos]);                  /* compression methods */
$end = $pos + 2 + unpack('n', $record, $pos)[1];
$extensions = [];
for ($pos += 2; $pos < $end; $pos += 4 + unpack('n', $record, $pos + 2)[1]) {
    $extensions[] = unpack('n', $record, $pos)[1];
}

var_dump(in_array(0x002b, $extensions, true)); /* supported_versions, sanity check */
var_dump(in_array(0xfe0d, $extensions, true)); /* encrypted_client_hello */
?>
--EXPECT--
int(0)
bool(true)
bool(false)
