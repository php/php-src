--TEST--
GHSA-r6x9-5r99-36j7 (Various packet overreads in mysqlnd_writeprotocol.c - auth switch response with an unterminated plugin name)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "secret";

$process = run_fake_server_in_background('auth_response_switch_unterminated');
$process->wait();

try {
    $conn = new mysqli( $servername, $username, $password, "", $process->getPort() );
} catch (Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

$process->terminate();

print "done!";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: 7d00000185a21a00000000c0080000000000000000000000000000000000000000000000726f6f7400140c7b6398a9794c7dc95737fa731fe62e95fe56626d7973716c5f6e61746976655f70617373776f7264002c0c5f636c69656e745f6e616d65076d7973716c6e640c5f7365727665725f686f7374093132372e302e302e31
[*] Sending - Malicious Auth Switch Response [plugin name not terminated]: 16000002fe6d7973716c5f6e61746976655f70617373776f7264

Warning: mysqli::__construct(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli::__construct(): AUTH_RESPONSE packet shorter than expected in %s on line %d
mysqlnd cannot connect to MySQL 4.1+ using the old insecure authentication. Please use an administration tool to reset your password with the command SET PASSWORD = PASSWORD('your_existing_password'). This will store a new, and more secure, hash value in mysql.user. If this user is used in other scripts executed by PHP 5.2 or earlier you might need to remove the old-passwords flag from your my.cnf file
done!
