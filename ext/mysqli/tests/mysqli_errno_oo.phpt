--TEST--
$mysqli->errno
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    $tmp    = NULL;
    $link   = NULL;

    $mysqli = new mysqli();
    if (0 !== ($tmp = @$mysqli->errno))
        printf("[001] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    var_dump($mysqli->errno);

    if (!$mysqli->query('DROP TABLE IF EXISTS test')) {
        printf("[003] Failed to drop old test table: [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    $mysqli->query('SELECT * FROM test');
    var_dump($mysqli->errno);

    @$mysqli->query('No SQL');
    if (($tmp = $mysqli->errno) === 0)
        printf("[004] Expecting int/any non zero got %s/%s\n", gettype($tmp), $tmp);

    $mysqli->close();

    try {
        $mysqli->errno;
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
