--TEST--
mysqli_stmt_param_count()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    try {
        mysqli_stmt_param_count($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    function func_test_mysqli_stmt_param_count($stmt, $query, $expected, $offset) {

        if (!mysqli_stmt_prepare($stmt, $query)) {
            printf("[%03d] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_error($stmt));
            return false;
        }

        if ($expected !== ($tmp = mysqli_stmt_param_count($stmt)))
            printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 3,
                gettype($expected), $expected,
                gettype($tmp), $tmp);
        return true;
    }

    func_test_mysqli_stmt_param_count($stmt, "SELECT 1 AS a", 0, 10);
    func_test_mysqli_stmt_param_count($stmt, "INSERT INTO test(id) VALUES (?)", 1, 20);
    func_test_mysqli_stmt_param_count($stmt, "INSERT INTO test(id, label) VALUES (?, ?)", 2, 30);
    func_test_mysqli_stmt_param_count($stmt, "INSERT INTO test(id, label) VALUES (?, '?')", 1, 40);

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_param_count($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_stmt object is not fully initialized
mysqli_stmt object is already closed
done!
