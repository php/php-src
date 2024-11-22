--TEST--
AUTH_RESPONSE packet is shorter than expected
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once __DIR__ . '/../fake_server.inc';

$port = 50001;
$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('ok_packet_too_short', $port);
$process->wait();

try {
    $conn = new mysqli( $servername, $username, $password, "", $port );
} catch (Exception $e) {
    echo 'ERROR: ' . $e->getMessage() . PHP_EOL;
}

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0600000200000002000000

Warning: mysqli::__construct(): Premature end of data (mysqlnd_wireprotocol.c:732) in %s
ERROR: AUTH_RESPONSE packet is shorter than expected
done!
