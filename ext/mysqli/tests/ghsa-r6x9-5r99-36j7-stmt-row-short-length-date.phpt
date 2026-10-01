--TEST--
GHSA-r6x9-5r99-36j7 (Various packet overreads in mysqlnd_writeprotocol.c - stmt row date field length too short for the type)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('stmt_response_row_short_length_date');
$process->wait();

$conn = new mysqli($servername, $username, $password, "", $process->getPort());

echo "[*] Prepare and execute on the fake server...\n";
$stmt = $conn->prepare("SELECT strval, datval FROM data");
$stmt->execute();
$result = $stmt->get_result();
while ($row = $result->fetch_row()) {
    var_dump($row);
}
$stmt->close();
$conn->close();

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0700000200000002000000
[*] Prepare and execute on the fake server...
[*] Received: 200000001653454c4543542073747276616c2c2064617476616c2046524f4d2064617461
[*] Sending - Stmt prepare data datval: 0c0000010001000000020000000000003200000203646566087068705f74657374046461746104646174610673747276616c0673747276616c0ce000c8000000fd01100000003200000303646566087068705f74657374046461746104646174610664617476616c0664617476616c0c3f000a0000000a811000000005000004fe00000200
[*] Received: 0a00000017010000000001000000
[*] Sending - Malicious Stmt Response for data datval [length too short]: 01000001023200000203646566087068705f74657374046461746104646174610673747276616c0673747276616c0ce000c8000000fd01100000003200000303646566087068705f74657374046461746104646174610664617476616c0664617476616c0c3f000a0000000a811000000005000004fe00002200090000050000047465737401de05000006fe00002200

Warning: mysqli_result::fetch_row(): Malformed server packet. Field length is too short for the field type in %s on line %d
done!
