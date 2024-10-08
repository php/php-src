--TEST--
GHSA-h35g-vwh6-m678 (mysqlnd leaks partial content of the heap - upsert filename buffer over-read)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$port = 33305;
$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('upsert_response_filename_over_read', $port);
$process->wait();

$conn = new mysqli($servername, $username, $password, "", $port);
echo "[*] Running query on the fake server...\n";

$result = $conn->query("SELECT * from users");
$info = mysqli_info($conn);

var_dump($info);

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0700000200000002000000
[*] Running query on the fake server...
[*] Received: 140000000353454c454354202a2066726f6d207573657273
[*] Sending - Malicious Tabular Response [Extract heap through buffer over-read]: 0900000100000000000000fa65

Warning: mysqli::query(): RSET_HEADER packet additional data length is past 249 bytes the packet size in %s on line %d

Warning: mysqli::query(): Error reading result set's header in %s on line %d
NULL
done!
