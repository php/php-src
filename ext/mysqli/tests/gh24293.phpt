--TEST--
GH-24293 (Segfault on connect with an empty password to a MariaDB PAM account)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('auth_switch_clear_password');
$process->wait();

$conn = new mysqli($servername, $username, $password, "", $process->getPort());
$conn->close();

$process->terminate(true);

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 6900000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400006d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Auth Switch Request [mysql_clear_password]: 2b000002fe6d7973716c5f636c6561725f70617373776f726400473e3f6047257c676c6b55463f49335f686c643100
[*] Received: 0100000300
[*] Sending - Server OK: 0700000400000002000000
[*] Server finished
done!
