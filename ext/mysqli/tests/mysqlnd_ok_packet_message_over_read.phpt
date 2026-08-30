--TEST--
mysqlnd OK packet message length buffer over-read
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('ok_packet_message_over_read');
$process->wait();

try {
    $conn = new mysqli( $servername, $username, $password, "", $process->getPort());
    var_dump($conn->select_db("test"));
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

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
[*] Sending - Malicious OK Packet [message length past the packet size]: %s

Warning: mysqli::select_db(): OK packet message length is past the packet size in %s on line %d

Warning: mysqli::select_db(): Error while reading INIT_DB's response packet. PID=%d in %s on line %d
mysqli_sql_exception: Malformed packet
done!
