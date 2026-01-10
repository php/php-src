--TEST--
mysqli insert (bind_param + bind_result) long char/text types
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

    mysqli_query($link,"CREATE TABLE insert_bind_char_text_long(c1 char(10), c2 text)");


    $stmt = mysqli_prepare($link, "INSERT INTO insert_bind_char_text_long VALUES (?,?)");
    mysqli_stmt_bind_param($stmt, "ss", $a1, $a2);

    $a1 = "1234567890";
    $a2 = str_repeat("A1", 32000);

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $stmt = mysqli_prepare($link, "SELECT * FROM insert_bind_char_text_long");
    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test[] = $c1;
    $test[] = ($a2 == $c2) ? "32K String ok" : "32K String failed";

    var_dump($test);

    mysqli_stmt_close($stmt);
    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('insert_bind_char_text_long');
?>
--EXPECTF--
array(2) {
  [0]=>
  string(10) "1234567890"
  [1]=>
  %s(13) "32K String ok"
}
done!
