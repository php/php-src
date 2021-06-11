--TEST--
mysqli_close()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    $tmp = $mysqli->close();
    if (true !== $tmp)
        printf("[003] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    try {
        $mysqli->close();
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        $mysqli->query("SELECT 1");
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
my_mysqli object is already closed
my_mysqli object is already closed
done!
