--TEST--
mysqli insert (bind_param + bind_result) integer types
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
        "CREATE TABLE insert_bind_integers(
            c1 int unsigned,
            c2 int unsigned,
            c3 int,
            c4 int,
            c5 int,
            c6 int unsigned,
            c7 int
        )"
    );

    $stmt = mysqli_prepare($link, "INSERT INTO insert_bind_integers VALUES (?,?,?,?,?,?,?)");
    mysqli_stmt_bind_param($stmt, "iiiiiii", $c1,$c2,$c3,$c4,$c5,$c6,$c7);
    $c1 = -23;
    $c2 = 35999;
    $c3 = NULL;
    $c4 = -500;
    $c5 = -9999999;
    $c6 = -0;
    $c7 = 0;

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_integers");
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
tear_down_table_on_default_connection('insert_bind_integers');
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
