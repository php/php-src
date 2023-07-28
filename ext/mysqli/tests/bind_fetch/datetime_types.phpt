--TEST--
mysqli bind_result datetimes
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

    $link = default_mysqli_connect();

    mysqli_query($link, "SET sql_mode=''");

try {
    $rc = mysqli_query($link,
        "CREATE TABLE test_bind_result_datetime(
        c1 date,
        c2 time,
        c3 timestamp(14),
        c4 year,
        c5 datetime,
        c6 timestamp(4),
        c7 timestamp(6)) ENGINE=" . get_default_db_engine()
    );
} catch (\mysqli_sql_exception) {
    /* 14 Too big precision for timestamp */
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
    $rc = mysqli_query($link,
        "CREATE TABLE test_bind_result_datetime(
        c1 date,
        c2 time,
        c3 timestamp,
        c4 year,
        c5 datetime,
        c6 timestamp(4),
        c7 timestamp(6)) ENGINE=" . get_default_db_engine()
    );
}

    $rc = mysqli_query($link, "INSERT INTO test_bind_result_datetime VALUES(
        '2002-01-02',
        '12:49:00',
        '2002-01-02 17:46:59',
        2010,
        '2010-07-10',
        '2020','1999-12-29')");
    if (!$rc)
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $stmt = mysqli_prepare($link, "SELECT c1, c2, c3, c4, c5, c6, c7 FROM test_bind_result_datetime");
    mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_result_datetime');
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
  string(4) "2010"
  [4]=>
  string(19) "2010-07-10 00:00:00"
  [5]=>
  string(24) "0000-00-00 00:00:00.0000"
  [6]=>
  string(26) "1999-12-29 00:00:00.000000"
}
done!
