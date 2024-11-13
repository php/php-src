--TEST--
mysqli_stmt_prepare()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    // Note: No SQL tests here! We can expand one of the *fetch()
    // tests to a generic SQL test, if we ever need that.
    // We would duplicate the SQL test cases if we have it here and in one of the
    // fetch tests, because the fetch tests would have to call prepare/execute etc.
    // anyway.

    require 'table.inc';

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (false !== ($tmp = mysqli_stmt_prepare($stmt, '')))
        printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== ($tmp = mysqli_stmt_prepare($stmt, 'SELECT id FROM test')))
        printf("[006] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_prepare($stmt, "SELECT id FROM test");
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
mysqli_stmt object is already closed
done!
