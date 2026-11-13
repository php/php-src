--TEST--
GH-23956 (Regression with MySQL 4.1 after mysqlnd security fix - 1-byte old password auth response)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('auth_response_old_password');
$process->wait();

try {
    new mysqli($servername, $username, $password, "", $process->getPort());
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage(), "\n";
}

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: %s
[*] Received: %s
[*] Sending - Auth Response [old password]: 01000002fe
mysqlnd cannot connect to MySQL 4.1+ using the old insecure authentication.%s
done!
