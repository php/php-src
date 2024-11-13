--TEST--
mysqli bind_result 1
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

mysqli_query(
    $link,
    "CREATE TABLE test_bind_fetch_varied(
        col1 tinyint, col2 smallint,
        col3 int, col4 bigint,
        col5 float, col6 double,
        col7 date, col8 time,
        col9 varbinary(10),
        col10 varchar(50),
        col11 char(20),
        col12 char(3) DEFAULT NULL
    ) ENGINE=" . get_default_db_engine()
);

mysqli_query($link, "INSERT INTO test_bind_fetch_varied
    VALUES(1, 2, 3, 4, 5.1, 6.2, '2020-02-21', '11:04', '111', 'foo1', 1000, null)");

$stmt = mysqli_prepare($link, "SELECT col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11, col12 from test_bind_fetch_varied ORDER BY col1");
mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8, $c9, $c10, $c11, $c12);
mysqli_stmt_execute($stmt);

mysqli_stmt_fetch($stmt);

$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$c10,$c11,$c12);

var_dump($test);

mysqli_stmt_close($stmt);
mysqli_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_varied');
?>
--EXPECT--
array(12) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  float(5.1)
  [5]=>
  float(6.2)
  [6]=>
  string(10) "2020-02-21"
  [7]=>
  string(8) "11:04:00"
  [8]=>
  string(3) "111"
  [9]=>
  string(4) "foo1"
  [10]=>
  string(4) "1000"
  [11]=>
  NULL
}
done!
