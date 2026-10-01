--TEST--
GHSA-r6x9-5r99-36j7 (Various packet overreads in mysqlnd_writeprotocol.c - rset header upsert affected_rows)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('upsert_rset_header_affected_rows_over_read');
$process->wait();

$conn = new mysqli($servername, $username, $password, "", $process->getPort());

// Construct the SQL query directly
$sql = "UPDATE data SET strval = 'test' WHERE id = 1";

// Execute the query
$result = $conn->query($sql);
if ($result) {
    echo "Affected rows: " . $conn->affected_rows . "\n";
    echo "Info: " . $conn->info . "\n";
}

// Close the connection
$conn->close();

$process->terminate(true);

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0700000200000002000000
[*] Received: 2d000000035550444154452064617461205345542073747276616c203d20277465737427205748455245206964203d2031
[*] Sending - Malicious Upsert Response [affected rows overflow]: 0200000100fd

Warning: mysqli::query(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli::query(): RSET_HEADER packet shorter than expected in %s on line %d

Warning: mysqli::query(): Error reading result set's header in %s on line %d
[*] Server finished
done!
