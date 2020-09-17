--TEST--
$mysqli->error
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    $mysqli = new mysqli();
    if ('' !== ($tmp = @$mysqli->error))
        printf("[001] Expecting empty string, got %s/'%s'\n", gettype($tmp), $tmp);

    if (!$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    $tmp = $mysqli->error;
    if (!is_string($tmp) || ('' !== $tmp))
        printf("[003] Expecting string/empty, got %s/%s. [%d] %s\n", gettype($tmp), $tmp, $mysqli->errno, $mysqli->error);

    if (!$mysqli->query('DROP TABLE IF EXISTS test')) {
        printf("[004] Failed to drop old test table: [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    $mysqli->query('SELECT * FROM test');
    $tmp = $mysqli->error;
    if (!is_string($tmp) || !preg_match("/Table '\w*\.test' doesn't exist/su", $tmp))
        printf("[006] Expecting string/[Table... doesn't exit], got %s/%s. [%d] %s\n", gettype($tmp), $tmp, $mysqli->errno, $mysqli->error);

    $mysqli->close();

    try {
        $mysqli->error;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
