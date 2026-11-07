--TEST--
mysqlnd: short metadata EOF packet on mysqli_stmt::prepare() after stmt_init()
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$process = run_fake_server_in_background('stmt_prepare_metadata_eof_short');
$process->wait();

$conn = new mysqli('127.0.0.1', 'root', '', '', $process->getPort());
$stmt = $conn->stmt_init();
var_dump($stmt->prepare('SELECT item FROM items'));
var_dump($stmt->errno);
unset($stmt);

$process->terminate(true);

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Server OK: 0700000200000002000000
[*] Received: 170000001653454c454354206974656d2046524f4d206974656d73
[*] Sending - Stmt prepare items with short EOF: 0c0000010001000000010000000000003000000203646566087068705f74657374056974656d73056974656d73046974656d046974656d0ce000c8000000fd011000000004000003fe000002

Warning: mysqli_stmt::prepare(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli_stmt::prepare(): EOF packet shorter than expected in %s on line %d
bool(false)
int(0)
[*] Received: 050000001901000000
[*] Server finished
done!
