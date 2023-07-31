--TEST--
mysqli insert (bind_param + bind_result) small integer types
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

    mysqli_query($link,"CREATE TABLE insert_bind_smallint(c1 smallint unsigned,
        c2 smallint unsigned,
        c3 smallint,
        c4 smallint,
        c5 smallint,
        c6 smallint unsigned,
        c7 smallint)");

    $stmt = mysqli_prepare($link, "INSERT INTO insert_bind_smallint VALUES (?,?,?,?,?,?,?)");
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

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_smallint");
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
tear_down_table_on_default_connection('insert_bind_smallint');
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
  int(-32768)
  [5]=>
  int(0)
  [6]=>
  int(0)
}
done!
