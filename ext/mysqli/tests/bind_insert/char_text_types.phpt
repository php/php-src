--TEST--
mysqli insert (bind_param + bind_result) char/text types
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

    mysqli_query($link,"CREATE TABLE insert_bind_char_text(c1 char(10), c2 text)");

    $stmt = mysqli_prepare($link, "INSERT INTO insert_bind_char_text VALUES (?,?)");
    mysqli_stmt_bind_param($stmt, "ss", $q1, $q2);
    $q1 = "1234567890";
    $q2 = "this is a test";
    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_char_text");
    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test = array($c1,$c2);

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('insert_bind_char_text');
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "1234567890"
  [1]=>
  string(14) "this is a test"
}
done!
