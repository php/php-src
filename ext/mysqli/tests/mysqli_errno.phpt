--TEST--
mysqli_errno()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
}

    var_dump(mysqli_errno($link));

    if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
        printf("[004] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    mysqli_query($link, 'SELECT * FROM test');
    var_dump(mysqli_errno($link));

    @mysqli_query($link, 'No SQL');
    if (($tmp = mysqli_errno($link)) == 0)
        printf("[005] Expecting int/any non zero got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    try {
        mysqli_errno($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECTF--
int(0)
int(%d)
mysqli object is already closed
done!
