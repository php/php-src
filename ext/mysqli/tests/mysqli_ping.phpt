--TEST--
mysqli_ping()
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

    var_dump(mysqli_ping($link));

    // provoke an error to check if mysqli_ping resets it
    $res = mysqli_query($link, 'SELECT * FROM unknown_table');
    if (!($errno = mysqli_errno($link)))
        printf("[003] Statement should have caused an error\n");

    var_dump(mysqli_ping($link));
    if ($errno === mysqli_errno($link))
        printf("[004] Error codes should have been reset\n");

    mysqli_close($link);

    try {
        mysqli_ping($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
bool(true)
bool(true)
mysqli object is already closed
done!
