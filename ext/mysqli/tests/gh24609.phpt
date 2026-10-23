--TEST--
GH-24609 (mysqlnd: OK packet without an info message rejected on x32 builds)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$process = run_fake_server_in_background('upsert_rset_header_no_info');
$process->wait();

mysqli_report(MYSQLI_REPORT_OFF);
$conn = new mysqli("127.0.0.1", "root", "", "", $process->getPort());
var_dump($conn->query("SET NAMES utf8"));
var_dump($conn->affected_rows);
$conn->close();

$process->terminate(true);

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: %s
[*] Received: %s
[*] Sending - Server OK: %s
[*] Received: %s
[*] Sending - Upsert Response without info: %s
bool(true)
int(0)
[*] Received: 0100000001
[*] Server finished
done!
