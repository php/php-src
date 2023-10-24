--TEST--
mysqli fetch float values
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

    // To get consistent result without depending on the DB version/setup
    mysqli_query($link, "SET sql_mode=''");

    mysqli_query(
        $link,
        "CREATE TABLE test_bind_fetch_float(
            c1 float(3),
            c2 float,
            c3 float unsigned,
            c4 float,
            c5 float,
            c6 float,
            c7 float(10) unsigned
        ) ENGINE=" . get_default_db_engine()
    );


    mysqli_query($link, "INSERT INTO test_bind_fetch_float (c1,c2,c3,c4,c5,c6,c7)
        VALUES (3.1415926535,-0.000001, -5, 999999999999, sin(0.6), 1.00000000000001, 888888888888888)");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_float");
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
tear_down_table_on_default_connection('test_bind_fetch_float');
?>
--EXPECT--
array(7) {
  [0]=>
  float(3.14159)
  [1]=>
  float(-1.0E-6)
  [2]=>
  float(0)
  [3]=>
  float(1000000000000)
  [4]=>
  float(0.564642)
  [5]=>
  float(1)
  [6]=>
  float(888889000000000)
}
done!
