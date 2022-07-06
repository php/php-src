--TEST--
Bug #35103 (Bad handling of unsigned bigint)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

$drop = <<<EOSQL
DROP TABLE test_bint;
DROP TABLE test_buint;
EOSQL;
    require_once("connect.inc");

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
    $mysql->query("DROP TABLE IF EXISTS test_bint");
    $mysql->query("CREATE TABLE test_bint (a bigint(20) default NULL) ENGINE=MYISAM");
    $mysql->query("INSERT INTO test_bint VALUES (9223372036854775807),(-9223372036854775808),(-2147483648),(-2147483649),(-2147483647),(2147483647),(2147483648),(2147483649)");

    $mysql->query("DROP TABLE IF EXISTS test_buint");
    $mysql->query("CREATE TABLE test_buint (a bigint(20) unsigned default NULL)");
    $mysql->query("INSERT INTO test_buint VALUES (18446744073709551615),(9223372036854775807),(9223372036854775808),(2147483647),(2147483649),(4294967295)");

    $stmt = $mysql->prepare("SELECT a FROM test_bint ORDER BY a");
    $stmt->bind_result($v);
    $stmt->execute();
    $i=0;
    echo "BIG INT SIGNED, TEST\n";
    while ($i++ < 8) {
        $stmt->fetch();
        echo $v, "\n";
    }
    $stmt->close();

    echo str_repeat("-", 20), "\n";

    $stmt = $mysql->prepare("SELECT a FROM test_buint ORDER BY a");
    $stmt->bind_result($v2);
    $stmt->execute();
    $j=0;
    echo "BIG INT UNSIGNED TEST\n";
    while ($j++ < 6) {
        $stmt->fetch();
        echo $v2, "\n";
    }
    $stmt->close();

    $mysql->multi_query($drop);

    $mysql->close();
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bint") || !mysqli_query($link, "DROP TABLE IF EXISTS test_buint"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
BIG INT SIGNED, TEST
-9223372036854775808
-2147483649
-2147483648
-2147483647
2147483647
2147483648
2147483649
9223372036854775807
--------------------
BIG INT UNSIGNED TEST
2147483647
2147483649
4294967295
9223372036854775807
9223372036854775808
18446744073709551615
