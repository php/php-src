--TEST--
mysqli_error()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    mysqli_query($link, "set names utf8");

    $tmp = mysqli_error($link);
    if (!is_string($tmp) || ('' !== $tmp))
        printf("[004] Expecting string/empty, got %s/%s. [%d] %s\n", gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));


    mysqli_query($link, 'SELECT * FROM няма_такава_таблица');
    $tmp = mysqli_error($link);
    var_dump(str_replace($db.".", "", $tmp));

    mysqli_close($link);

    try {
        mysqli_error($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECTF--
string(%d) "Table 'няма_такава_таблица' doesn't exist"
mysqli object is already closed
done!
