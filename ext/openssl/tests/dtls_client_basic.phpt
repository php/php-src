--TEST--
dtls:// client: transport, UDP socket and DTLS setup
--SKIPIF--
<?php require __DIR__ . '/dtls_skipif.inc'; ?>
--FILE--
<?php
// Client-side dtls:// basics. This scenario grows as the transport is built out:
// the DTLS handshake and an application-data round trip are wired in later steps.
// For now it covers transport registration plus UDP socket and DTLS object setup
// (a UDP connect() succeeds with no peer listening, the handshake is not run yet).
$errno = $errstr = null;
$client = stream_socket_client('dtls://127.0.0.1:44444', $errno, $errstr);
var_dump($client !== false);
var_dump($errno);
if ($client !== false) {
    fclose($client);
}
?>
--EXPECT--
bool(true)
int(0)
