--TEST--
mysqli_character_set_name(), mysql_client_encoding() [alias]
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    /* NOTE: http://bugs.mysql.com/bug.php?id=7923 makes this test fail very likely on all 4.1.x - 5.0.x! */
    require_once("connect.inc");

    $tmp	= NULL;
    $link	= NULL;

    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$res = $mysqli->query('SELECT version() AS server_version'))
        printf("[003] [%d] %s\n", $mysqli->errno, $mysqli->error);
    $tmp = $res->fetch_assoc();
    $res->free_result();
    $version = explode('.', $tmp['server_version']);
    if (empty($version))
        printf("[006] Cannot determine server version, need MySQL Server 4.1+ for the test!\n");

    if ($version[0] <= 4 && $version[1] < 1)
        printf("[007] Need MySQL Server 4.1+ for the test!\n");

    if (!$res = $mysqli->query('SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
        printf("[008] [%d] %s\n", $mysqli->errno, $mysqli->error);
    $tmp = $res->fetch_assoc();
    $res->free_result();
    if (!$tmp['charset'])
        printf("[009] Cannot determine current character set and collation\n");

    $charset = $mysqli->character_set_name();
    if ($tmp['charset'] !== $charset) {
        if ($tmp['collation'] === $charset) {
            printf("[010] Could be known server bug http://bugs.mysql.com/bug.php?id=7923, collation %s instead of character set returned, expected string/%s, got %s/%s\n",
                $tmp['collation'], $tmp['charset'], gettype($charset), $charset);
        } else {
            printf("[011] Expecting character set %s/%s, got %s/%s\n", gettype($tmp['charset']), $tmp['charset'], gettype($charset), $charset);
        }
    }

    $charset2 = $mysqli->character_set_name();
    if ($charset2 !== $charset) {
        printf("[012] Alias mysqli_character_set_name returned %s/%s, expected  %s/%s\n",
            gettype($charset2), $charset2, gettype($charset), $charset);
    }

    $mysqli->close();

    try {
        $mysqli->character_set_name();
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        $mysqli->character_set_name();
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
my_mysqli object is already closed
my_mysqli object is already closed
done!
