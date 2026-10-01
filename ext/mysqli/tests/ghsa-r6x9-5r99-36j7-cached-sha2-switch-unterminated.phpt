--TEST--
GHSA-r6x9-5r99-36j7 (Various packet overreads in mysqlnd_writeprotocol.c - cached sha2 result with an unterminated plugin name)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (!extension_loaded("openssl"))
    die("skip mysqlnd is built without SSL support, so the caching_sha2_password plugin is not available");
?>
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "secret";

$process = run_fake_server_in_background('cached_sha2_switch_unterminated');
$process->wait();

$conn = new mysqli( $servername, $username, $password, "", $process->getPort() );
echo "[*] connect_errno: ", $conn->connect_errno, "\n";
$conn->close();

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c64310063616368696e675f736861325f70617373776f7264
[*] Received: 8900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400204829cef922c9e8d5c6b4a4299cb857d65b18323a1b833559f3aa6a0b462994be63616368696e675f736861325f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Malicious Cached SHA2 Result [plugin name not terminated]: 16000002fe6d7973716c5f6e61746976655f70617373776f72640700000300000002000000

Warning: mysqli::__construct(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli::__construct(): SHA256_PK_REQUEST_RESPONSE packet shorter than expected in %s on line %d
[*] connect_errno: 0
done!
