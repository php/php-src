--TEST--
GH-23956 (Regression with MySQL 4.1 after mysqlnd security fix - 1-byte EOF packet)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('query_response_eof_one_byte');
$process->wait();

$conn = new mysqli($servername, $username, $password, "", $process->getPort());

echo "[*] Query the fake server...\n";
$result = $conn->query("SELECT strval, strval FROM data");

while ($row = $result->fetch_row()) {
    var_dump($row);
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
[*] Query the fake server...
[*] Received: %s
[*] Sending - Query Response [1-byte EOF packet]: %s
array(2) {
  [0]=>
  string(4) "test"
  [1]=>
  string(4) "test"
}
done!
