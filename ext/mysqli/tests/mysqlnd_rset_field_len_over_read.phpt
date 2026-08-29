--TEST--
mysqlnd result set field metadata string length buffer over-read (len clamped to packet size)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('rset_field_metadata_len_over_read');
$process->wait();

try {
    $conn = new mysqli( $servername, $username, $password, "", $process->getPort());
    var_dump($conn->query("SELECT * from users"));
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

$conn->close();

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: %s
[*] Sending - Server OK: 0700000200000002000000
[*] Received: %s
[*] Sending - Malicious Tabular Response [metadata string length past the packet size]: 01000001010c00000201610162016301640165fcff

Warning: mysqli::query(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli::query(): Result set field packet %d bytes shorter than expected in %s on line %d
bool(false)
done!
