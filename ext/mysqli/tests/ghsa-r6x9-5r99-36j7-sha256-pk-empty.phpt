--TEST--
GHSA-r6x9-5r99-36j7 (Various packet overreads in mysqlnd_writeprotocol.c - sha256 public key response with empty packet)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (!extension_loaded("openssl"))
    die("skip mysqlnd is built without SSL support, so the sha256_password plugin is not available");
?>
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "secret";

$process = run_fake_server_in_background('sha256_pk_response_empty');
$process->wait();

echo "[*] Connecting to the fake server...\n";
$conn = new mysqli($servername, $username, $password, "", $process->getPort());

$conn->close();

$process->terminate(true);

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connecting to the fake server...
[*] Connection established
[*] Sending - Server Greeting: 520000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431007368613235365f70617373776f7264
[*] Received: 0100000101
[*] Sending - Malicious SHA256 PK Response [empty packet]: 00000002

Warning: mysqli::__construct(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli::__construct(): SHA256_PK_REQUEST_RESPONSE packet shorter than expected in %s on line %d

Warning: Error while receiving public key. PID=%d in %s on line %d
[*] Received: 6300000385a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400007368613235365f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0700000400000002000000
[*] Received: 0100000001
[*] Server finished
done!
