--TEST--
Fix bug #67563 (mysqli compiled with mysqlnd does not take ipv6 adress as parameter)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';

if (@stream_socket_client('udp://[::1]:8888') === false)
    die('skip no IPv6 support');

if (!$link = @my_mysqli_connect('[::1]', $user, $passwd, $db, $port, $socket)) {
    die(sprintf("SKIP Cannot connect to the server using host=[::1], user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $user, $db, $port, $socket));
}
?>
--INI--
max_execution_time=240
--FILE--
<?php
require_once 'connect.inc';

$hosts = ['::1', "[::1]:$port"];

foreach ($hosts as $host) {
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=%s dbname=%s, port=%s, socket=%s\n",
            $host, $user, $passwd, $db, $port, $socket);
    } else {
        $link->close();
    }
}

print "done!";
?>
--EXPECT--
done!
