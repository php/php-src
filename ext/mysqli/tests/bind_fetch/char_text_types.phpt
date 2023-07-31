--TEST--
mysqli fetch char/text
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

    if (!mysqli_query($link,"CREATE TABLE test_bind_fetch_char(c1 char(10), c2 text) ENGINE=" . get_default_db_engine()))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test_bind_fetch_char VALUES ('1234567890', 'this is a test0')"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test_bind_fetch_char VALUES ('1234567891', 'this is a test1')"))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test_bind_fetch_char VALUES ('1234567892', 'this is a test2')"))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test_bind_fetch_char VALUES ('1234567893', 'this is a test3')"))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_char ORDER BY c1"))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $c1 = $c2 = NULL;
    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_execute($stmt);
    $i = 4;
    while ($i--) {
        mysqli_stmt_fetch($stmt);
        $test = array($c1, $c2);
        var_dump($test);
    }

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_char');
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "1234567890"
  [1]=>
  string(15) "this is a test0"
}
array(2) {
  [0]=>
  string(10) "1234567891"
  [1]=>
  string(15) "this is a test1"
}
array(2) {
  [0]=>
  string(10) "1234567892"
  [1]=>
  string(15) "this is a test2"
}
array(2) {
  [0]=>
  string(10) "1234567893"
  [1]=>
  string(15) "this is a test3"
}
done!
