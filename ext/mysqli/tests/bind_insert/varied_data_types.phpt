--TEST--
mysqli insert (bind_param + bind_result)
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

    $rc = mysqli_query($link,"CREATE TABLE insert_bind_varied1(col1 tinyint, col2 smallint,
                                                    col3 int, col4 bigint,
                                                    col5 float, col6 double,
                                                    col7 date, col8 time,
                                                    col9 varbinary(10),
                                                    col10 varchar(50),
                                                    col11 char(20)) ENGINE=" . get_default_db_engine());
    if (!$rc)
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$stmt = mysqli_prepare($link, "INSERT INTO insert_bind_varied1(col1,col10, col11, col6) VALUES (?,?,?,?)"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_stmt_bind_param($stmt, "issd", $c1, $c2, $c3, $c4);

    $c1 = 1;
    $c2 = "foo";
    $c3 = "foobar";
    $c4 = 3.14;

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_varied1");

    mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8, $c9, $c10, $c11);
    mysqli_stmt_execute($stmt);

    mysqli_stmt_fetch($stmt);

    $test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$c10,$c11);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('insert_bind_varied1');
?>
--EXPECT--
array(11) {
  [0]=>
  int(1)
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  float(3.14)
  [6]=>
  NULL
  [7]=>
  NULL
  [8]=>
  NULL
  [9]=>
  string(3) "foo"
  [10]=>
  string(6) "foobar"
}
done!
