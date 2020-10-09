--TEST--
mysqli connect
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_query($link, "SET sql_mode=''");

    if (!mysqli_query($link,"DROP TABLE IF EXISTS test_bind_result"))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $rc = @mysqli_query($link,"CREATE TABLE test_bind_result(
        c1 date,
        c2 time,
        c3 timestamp(14),
        c4 year,
        c5 datetime,
        c6 timestamp(4),
        c7 timestamp(6)) ENGINE=" . $engine);

    /*
    Seems that not all MySQL 6.0 installations use defaults that ignore the display widths.
    From the manual:
    From MySQL 4.1.0 on, TIMESTAMP display format differs from that of earlier MySQL releases:
    [...]
    Display widths (used as described in the preceding section) are no longer supported.
    In other words, for declarations such as TIMESTAMP(2), TIMESTAMP(4), and so on,
    the display width is ignored.
    [...]
    */
    if (!$rc)
        $rc = @mysqli_query($link,"CREATE TABLE test_bind_result(
            c1 date,
            c2 time,
            c3 timestamp,
            c4 year,
            c5 datetime,
            c6 timestamp,
            c7 timestamp) ENGINE=" . $engine);

    if (!$rc)
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $rc = mysqli_query($link, "INSERT INTO test_bind_result VALUES(
        '2002-01-02',
        '12:49:00',
        '2002-01-02 17:46:59',
        2010,
        '2010-07-10',
        '2020','1999-12-29')");
    if (!$rc)
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $stmt = mysqli_prepare($link, "SELECT c1, c2, c3, c4, c5, c6, c7 FROM test_bind_result");
    mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_query($link, "DROP TABLE IF EXISTS test_bind_result");
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_result"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
array(7) {
  [0]=>
  string(10) "2002-01-02"
  [1]=>
  string(8) "12:49:00"
  [2]=>
  string(19) "2002-01-02 17:46:59"
  [3]=>
  int(2010)
  [4]=>
  string(19) "2010-07-10 00:00:00"
  [5]=>
  string(19) "0000-00-00 00:00:00"
  [6]=>
  string(19) "1999-12-29 00:00:00"
}
done!
