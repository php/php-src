--TEST--
mysqli fetch mixed values 2
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

    mysqli_query(
        $link,
        "CREATE TABLE test_bind_fetch_varied3(
            c1 tinyint, c2 smallint,
            c3 int, c4 bigint,
            c5 float, c6 double,
            c7 varbinary(10),
            c8 varchar(10)
        ) ENGINE=" . get_default_db_engine()
    );

    mysqli_query($link, "INSERT INTO test_bind_fetch_varied3
        VALUES(120,2999,3999,54,2.6,58.89,'206','6.7')");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_varied3");
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
tear_down_table_on_default_connection('test_bind_fetch_varied3');
?>
--EXPECT--
array(8) {
  [0]=>
  int(120)
  [1]=>
  int(2999)
  [2]=>
  int(3999)
  [3]=>
  int(54)
  [4]=>
  float(2.6)
  [5]=>
  float(58.89)
  [6]=>
  string(3) "206"
  [7]=>
  string(3) "6.7"
}
done!
