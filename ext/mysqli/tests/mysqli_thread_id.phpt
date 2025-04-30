--TEST--
mysqli_thread_id()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
        exit(1);
    }

    if (!is_int($tmp = mysqli_thread_id($link)) || (0 === $tmp))
        printf("[003] Expecting int/any but zero, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    // should work if the thread id is correct
    $link->real_query('KILL '.mysqli_thread_id($link));
    // We kill our own connection so we should get "Query execution was interrupted"
	if ($link->errno !== 1317)
		printf("[042] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    
    mysqli_close($link);

    try {
        mysqli_thread_id($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
