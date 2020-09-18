--TEST--
mysqli_stmt_init()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    /*
    NOTE: no datatype tests here! This is done by
    mysqli_stmt_bind_result.phpt already. Restrict
    this test case to the basics.
    */
    require_once("connect.inc");

    require('table.inc');

    if (!is_object($stmt = mysqli_stmt_init($link)))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($stmt2 = @mysqli_stmt_init($link)))
        printf("[003a] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    try {
        mysqli_stmt_close($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    try {
        mysqli_stmt_init($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_stmt object is not fully initialized
mysqli object is already closed
done!
