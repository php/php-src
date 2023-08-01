--TEST--
mysqli_stat()
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

    if ((!is_string($tmp = mysqli_stat($link))) || ('' === $tmp))
        printf("[004] Expecting non empty string, got %s/'%s', [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysql_error($link));

    mysqli_close($link);

    try {
        mysqli_stat($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
