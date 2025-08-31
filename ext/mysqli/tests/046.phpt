--TEST--
mysqli_stmt_affected_rows (delete)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);

    mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
    mysqli_query($link, "CREATE TABLE test_affected (foo int) ENGINE=" . $engine);

    mysqli_query($link, "INSERT INTO test_affected VALUES (1),(2),(3),(4),(5)");

    $stmt = mysqli_prepare($link, "DELETE FROM test_affected WHERE foo=?");
    mysqli_stmt_bind_param($stmt, "i", $c1);

    $c1 = 2;

    mysqli_stmt_execute($stmt);
    $x = mysqli_stmt_affected_rows($stmt);

    mysqli_stmt_close($stmt);
    var_dump($x==1);

    mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_affected"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
bool(true)
done!
