--TEST--
mysqli_close()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

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
