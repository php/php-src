--TEST--
mysqli fetch integer values
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
        "CREATE TABLE test_bind_fetch_integers(
            c1 int unsigned,
            c2 int unsigned,
            c3 int,
            c4 int,
            c5 int,
            c6 int unsigned,
            c7 int
        ) ENGINE=" . get_default_db_engine()
    );

    mysqli_query($link, "INSERT INTO test_bind_fetch_integers VALUES (-23,35999,NULL,-500,-9999999,-0,0)");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_integers");
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
tear_down_table_on_default_connection('test_bind_fetch_integers');
?>
--EXPECT--
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(35999)
  [2]=>
  NULL
  [3]=>
  int(-500)
  [4]=>
  int(-9999999)
  [5]=>
  int(0)
  [6]=>
  int(0)
}
done!
