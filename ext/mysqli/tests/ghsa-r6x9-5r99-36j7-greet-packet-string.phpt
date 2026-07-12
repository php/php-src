--TEST--
GHSA-r6x9-5r99-36j7 (Various packet overreads in mysqlnd_writeprotocol.c - greet packet over-read string)
--EXTENSIONS--
mysqli
--FILE--
<?php
require_once 'fake_server.inc';

$servername = "127.0.0.1";
$username = "root";
$password = "";

$process = run_fake_server_in_background('greet_over_read_string');
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
[*] Sending - Malicious Server Greeting: 080000000a352e3503030303473e3f6047257c67

Warning: mysqli::__construct(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli::__construct(): GREET packet shorter than expected in %s on line %d

Warning: mysqli::__construct(): Error while reading greeting packet. PID=%d in %s on line %d
Unknown error while trying to connect via tcp://127.0.0.1:%d
done!
