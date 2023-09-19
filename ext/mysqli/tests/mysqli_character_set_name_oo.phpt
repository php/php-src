--TEST--
mysqli_character_set_name()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
/* NOTE: http://bugs.mysql.com/bug.php?id=7923 makes this test fail very likely on all 4.1.x - 5.0.x! */
require_once 'connect.inc';

$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

if (!$res = $mysqli->query('SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
    printf("[001] [%d] %s\n", $mysqli->errno, $mysqli->error);
$tmp = $res->fetch_assoc();
$res->free_result();
if (!$tmp['charset'])
    printf("[002] Cannot determine current character set and collation\n");

$charset = $mysqli->character_set_name();
if ($tmp['charset'] !== $charset) {
    if ($tmp['collation'] === $charset) {
        printf("[003] Could be known server bug http://bugs.mysql.com/bug.php?id=7923, collation %s instead of character set returned, expected string/%s, got %s/%s\n",
            $tmp['collation'], $tmp['charset'], gettype($charset), $charset);
    } else {
        printf("[004] Expecting character set %s/%s, got %s/%s\n", gettype($tmp['charset']), $tmp['charset'], gettype($charset), $charset);
    }
}

$mysqli->close();

try {
    $mysqli->character_set_name();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

print "done!";
?>
--EXPECT--
my_mysqli object is already closed
done!
