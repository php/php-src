--TEST--
mysqli fetch long char/text
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

    if (!mysqli_query($link, "CREATE TABLE test_bind_fetch_char_long(c1 char(10), c2 text) ENGINE=" . get_default_db_engine()))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $a = str_repeat("A1", 32000);

    mysqli_query($link, "INSERT INTO test_bind_fetch_char_long VALUES ('1234567890', '$a')");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_char_long");
    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    $test[] = $c1;
    $test[] = ($a == $c2) ? "32K String ok" : "32K String failed";

    var_dump($test);

    mysqli_stmt_fetch($stmt);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_char_long');
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "1234567890"
  [1]=>
  string(13) "32K String ok"
}
done!
