--TEST--
mysqlnd result set field metadata string length exceeds remaining packet bytes
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('rset_field_metadata_len_past_packet');
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
[*] Sending - Server Greeting: %s
[*] Received: %s
[*] Sending - Server OK: %s
[*] Received: %s
[*] Sending - Malicious Tabular Response [metadata string length past the packet size]: %s

Warning: mysqli::query(): Result set field metadata string length is past the packet size in %s on line %d
bool(false)
done!
