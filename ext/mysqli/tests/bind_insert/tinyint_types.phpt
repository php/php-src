--TEST--
mysqli insert (bind_param + bind_result) tiny integer types
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

    mysqli_select_db($link, get_default_database());
    mysqli_query($link, "SET sql_mode=''");

    mysqli_query($link,"CREATE TABLE insert_bind_tinyint(c1 tinyint,
        c2 tinyint unsigned,
        c3 tinyint not NULL,
        c4 tinyint,
        c5 tinyint,
        c6 tinyint unsigned,
        c7 tinyint)");

    $stmt = mysqli_prepare ($link, "INSERT INTO insert_bind_tinyint VALUES(?,?,?,?,?,?,?)");
    mysqli_stmt_bind_param($stmt, "iiiiiii", $c1,$c2,$c3,$c4,$c5,$c6,$c7);

    $c1 = -23;
    $c2 = 300;
    $c3 = 0;
    $c4 = -100;
    $c5 = -127;
    $c6 = 30;
    $c7 = 0;

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $c1 = $c2 = $c3 = $c4 = $c5 = $c6 = $c7 = NULL;

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_tinyint");
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
tear_down_table_on_default_connection('insert_bind_tinyint');
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
