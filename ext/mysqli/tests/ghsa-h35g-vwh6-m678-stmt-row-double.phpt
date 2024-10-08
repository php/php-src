--TEST--
GHSA-h35g-vwh6-m678 (mysqlnd leaks partial content of the heap - stmt row double buffer over-read)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$port = 33305;
$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('stmt_response_row_over_read_double', $port);
$process->wait();

$conn = new mysqli($servername, $username, $password, "", $port);

echo "[*] Preparing statement on the fake server...\n";
$stmt = $conn->prepare("SELECT strval, dblval FROM data");

$stmt->execute();
$result = $stmt->get_result();

// Fetch and display the results
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        var_dump($row["dblval"]);
    }
}
$stmt->close();
$conn->close();

$process->terminate(true);

print "done!";
?>
--EXPECTF--
[*] Server started
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0700000200000002000000
[*] Preparing statement on the fake server...
[*] Received: 200000001653454c4543542073747276616c2c2064626c76616c2046524f4d2064617461
[*] Sending - Stmt prepare data dblval: 0c0000010001000000020000000000003200000203646566087068705f74657374046461746104646174610673747276616c0673747276616c0ce000c8000000fd01100000003200000303646566087068705f74657374046461746104646174610664626c76616c0664626c76616c0c3f00160000000501101f000005000004fe00000200
[*] Received: 0a00000017010000000001000000
[*] Sending - Malicious Stmt Response for data dblval [Extract heap through buffer over-read]: 01000001023200000203646566087068705f74657374046461746104646174610673747276616c0673747276616c0ce000c8000000fd01100000003200000303646566087068705f74657374046461746104646174610664626c76616c0664626c76616c0c3f00160000000501101f000005000004fe000022000f00000500000674657374333333333333f33f05000006fe00002200

Warning: mysqli_result::fetch_assoc(): Malformed server packet. Field length pointing after the end of packet in %s on line %d
[*] Received: 0500000019010000000100000001
[*] Server finished
done!
