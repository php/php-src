--TEST--
Fix bug #67563 (mysqli compiled with mysqlnd does not take ipv6 adress as parameter)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';

if ($host !== '127.0.0.1') die('skip local test');

if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("SKIP Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket));
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
        printf("[001] Cannot connect to the server using host=%s, user=pamtest, passwd=pamtest dbname=%s, port=%s, socket=%s\n",
            $host, $db, $port, $socket);
    } else {
        $link->close();
    }
}

print "done!";
?>
--EXPECT--
done!
