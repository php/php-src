--TEST--
Error while reading greeting packet
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once __DIR__ . '/../fake_server.inc';

$port = 50001;
$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('invalid_greeting_packet', $port);
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
ERROR: Error while reading greeting packet
done!
