--TEST--
multiple binds
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

    mysqli_query($link,"DROP TABLE IF EXISTS mbind");
    mysqli_query($link,"CREATE TABLE mbind (a int, b varchar(10))");

    $stmt = mysqli_prepare($link, "INSERT INTO mbind VALUES (?,?)");

    mysqli_stmt_bind_param($stmt, "is", $a, $b);

    $a = 1;
    $b = "foo";

    mysqli_stmt_execute($stmt);

    mysqli_stmt_bind_param($stmt, "is", $c, $d);

    $c = 2;
    $d = "bar";

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM mbind");
    mysqli_stmt_execute($stmt);

    mysqli_stmt_bind_result($stmt, $e, $f);
    mysqli_stmt_fetch($stmt);

    mysqli_stmt_bind_result($stmt, $g, $h);
    mysqli_stmt_fetch($stmt);

    var_dump((array($e,$f,$g,$h)));

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
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(3) "foo"
  [2]=>
  int(2)
  [3]=>
  string(3) "bar"
}
done!
