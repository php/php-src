--TEST--
mysqli fetch tinyint values
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
        "CREATE TABLE test_bind_fetch_integers_tiny(
            c1 tinyint,
            c2 tinyint unsigned,
            c3 tinyint not NULL,
            c4 tinyint,
            c5 tinyint,
            c6 tinyint unsigned,
            c7 tinyint
        ) ENGINE=" . get_default_db_engine()
    );

    mysqli_query($link, "INSERT INTO test_bind_fetch_integers_tiny VALUES (-23,300,0,-100,-127,+30,0)");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_integers_tiny");
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
tear_down_table_on_default_connection('test_bind_fetch_integers_tiny');
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
