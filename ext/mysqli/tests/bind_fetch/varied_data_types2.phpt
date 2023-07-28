--TEST--
mysqli fetch mixed values
--INI--
precision=12
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

    $rc = mysqli_query($link, "CREATE TABLE test_bind_fetch_varied2(c1 tinyint, c2 smallint,
                                                        c3 int, c4 bigint,
                                                        c5 float, c6 double,
                                                        c7 varbinary(10),
                                                        c8 varchar(50)) ENGINE=" . get_default_db_engine());
    if (!$rc)
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $rc = mysqli_query($link,"INSERT INTO test_bind_fetch_varied2 VALUES(19,2999,3999,4999999,
                                                              2345.6,5678.89563,
                                                              'foobar','mysql rulez')");
    if (!$rc)
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_varied2");
    mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_varied2');
?>
--EXPECT--
array(8) {
  [0]=>
  int(19)
  [1]=>
  int(2999)
  [2]=>
  int(3999)
  [3]=>
  int(4999999)
  [4]=>
  float(2345.6)
  [5]=>
  float(5678.89563)
  [6]=>
  string(6) "foobar"
  [7]=>
  string(11) "mysql rulez"
}
done!
