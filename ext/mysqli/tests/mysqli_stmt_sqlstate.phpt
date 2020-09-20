--TEST--
mysqli_stmt_sqlstate()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if (!$stmt = mysqli_stmt_init($link))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    try {
        mysqli_stmt_sqlstate($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test"))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ('00000' !== ($tmp = mysqli_stmt_sqlstate($stmt)))
        printf("[007] Expecting string/00000, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (mysqli_stmt_prepare($stmt, "SELECT believe_me FROM i_dont_belive_that_this_table_exists"))
        printf("[008] Should fail! [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ('' === ($tmp = mysqli_stmt_sqlstate($stmt)))
        printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_sqlstate($stmt);
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
