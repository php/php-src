--TEST--
mysqli insert (bind_param + bind_result) with send_long_data
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

    mysqli_query($link,"CREATE TABLE insert_bind_send_long_data(c1 varchar(10), c2 text)");

    $stmt = mysqli_prepare ($link, "INSERT INTO insert_bind_send_long_data VALUES (?,?)");
    mysqli_stmt_bind_param($stmt, "sb", $c1, $c2);

    $c1 = "Hello World";

    mysqli_stmt_send_long_data($stmt, 1, "This is the first sentence.");
    mysqli_stmt_send_long_data($stmt, 1, " And this is the second sentence.");
    mysqli_stmt_send_long_data($stmt, 1, " And finally this is the last sentence.");

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_send_long_data");
    mysqli_stmt_bind_result($stmt, $d1, $d2);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test = array($d1,$d2);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('insert_bind_send_long_data');
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "Hello Worl"
  [1]=>
  string(99) "This is the first sentence. And this is the second sentence. And finally this is the last sentence."
}
done!
