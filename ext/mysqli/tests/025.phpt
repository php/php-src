--TEST--
mysqli bind_param/bind_result tinyint values
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
    mysqli_query($link, "SET sql_mode=''");

    mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
    mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 tinyint,
        c2 tinyint unsigned,
        c3 tinyint not NULL,
        c4 tinyint,
        c5 tinyint,
        c6 tinyint unsigned,
        c7 tinyint)");

    $stmt = mysqli_prepare ($link, "INSERT INTO test_bind_fetch VALUES(?,?,?,?,?,?,?)");
    mysqli_stmt_bind_param($stmt, "iiiiiii", $c1,$c2,$c3,$c4,$c5,$c6,$c7);

    $c1 = -23;
    $c2 = 300;
    $c3 = 0;
    $c4 = -100;
    $c5 = -127;
    $c6 = 30;
    $c7 = 0;

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    mysqli_query($link, "INSERT INTO test_bind_fetch VALUES (-23,300,0,-100,-127,+30,0)");

    $c1 = $c2 = $c3 = $c4 = $c5 = $c6 = $c7 = NULL;

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
    mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch");
    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
array(7) {
  [0]=>
  int(-23)
  [1]=>
  int(255)
  [2]=>
  int(0)
  [3]=>
  int(-100)
  [4]=>
  int(-127)
  [5]=>
  int(30)
  [6]=>
  int(0)
}
done!
