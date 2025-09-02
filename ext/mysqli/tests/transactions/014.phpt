--TEST--
mysqli autocommit/commit/rollback with innodb with CACHE
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_innodb_support_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

    $link = default_mysqli_connect();

    mysqli_autocommit($link, true);
    mysqli_query($link,"CREATE TABLE test_transactions_14(a int, b varchar(10)) Engine=InnoDB");
    mysqli_query($link, "INSERT INTO test_transactions_14 VALUES (1, 'foobar')");
    mysqli_autocommit($link, false);

    /* Modify data in DB */
    mysqli_query($link, "DELETE FROM test_transactions_14");
    mysqli_query($link, "INSERT INTO test_transactions_14 VALUES (2, 'egon')");

    /* Attempt to rollback */
    mysqli_rollback($link);

    /* Check if rollback was successful */
    $result = mysqli_query($link, "SELECT * FROM test_transactions_14");
    printf("Num_of_rows=%d\n", mysqli_num_rows($result));
    $row = mysqli_fetch_row($result);
    mysqli_free_result($result);

    var_dump($row);

    /* Modify data in DB */
    mysqli_query($link, "DELETE FROM test_transactions_14");
    mysqli_query($link, "INSERT INTO test_transactions_14 VALUES (2, 'egon')");

    /* Commit modifications */
    mysqli_commit($link);

    /* Check if commit was successful */
    $result = mysqli_query($link, "SELECT * FROM test_transactions_14");
    $row = mysqli_fetch_row($result);
    mysqli_free_result($result);

    var_dump($row);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_transactions_14');
?>
--EXPECT--
Num_of_rows=1
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(6) "foobar"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(4) "egon"
}
done!
