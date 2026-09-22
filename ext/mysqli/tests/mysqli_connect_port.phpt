--TEST--
mysqli_connect() with port in host
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

	// using port / host arguments
    if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
	}

    mysqli_close($link);

	// using port in host
    if (!$link = mysqli_connect("$host:$port", $user, $passwd, $db, "1$port", $socket)) {
        printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            "$host:$port", $user, $db, "1$port", $socket);
	}

    mysqli_close($link);
?>
Done
--EXPECTF--
Done
