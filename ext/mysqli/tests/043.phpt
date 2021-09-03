--TEST--
mysqli_stmt_bind_param (UPDATE)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);

    mysqli_query($link,"DROP TABLE IF EXISTS test_update");
    mysqli_query($link,"CREATE TABLE test_update(a varchar(10),
        b int) ENGINE=" . $engine);

    mysqli_query($link, "INSERT INTO test_update VALUES ('foo', 2)");

    $stmt = mysqli_prepare($link, "UPDATE test_update SET a=?,b=? WHERE b=?");
    mysqli_stmt_bind_param($stmt, "sii", $c1, $c2, $c3);

    $c1 = "Rasmus";
    $c2 = 1;
    $c3 = 2;

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $result = mysqli_query($link, "SELECT concat(a, ' is No. ', b) FROM test_update");
    $test = mysqli_fetch_row($result);
    mysqli_free_result($result);

    var_dump($test);

    mysqli_query($link, "DROP TABLE IF EXISTS test_update");
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_update"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
array(1) {
  [0]=>
  string(15) "Rasmus is No. 1"
}
done!
