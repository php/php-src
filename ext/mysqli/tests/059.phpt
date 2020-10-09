--TEST--
sqlmode + bind
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);

    mysqli_query($link, "SET SQL_MODE='PIPES_AS_CONCAT'");

    mysqli_query($link,"DROP TABLE IF EXISTS mbind");
    mysqli_query($link,"CREATE TABLE mbind (b varchar(25))");

    $stmt = mysqli_prepare($link, "INSERT INTO mbind VALUES (?||?)");

    mysqli_stmt_bind_param($stmt, "ss", $a, $b);

    $a = "foo";
    $b = "bar";

    mysqli_stmt_execute($stmt);

    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM mbind");
    mysqli_stmt_execute($stmt);

    mysqli_stmt_bind_result($stmt, $e);
    mysqli_stmt_fetch($stmt);

    var_dump($e);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS mbind"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
string(6) "foobar"
done!
