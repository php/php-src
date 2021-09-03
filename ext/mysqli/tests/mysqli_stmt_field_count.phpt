--TEST--
mysqli_stmt_field_counts()
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

    $stmt = mysqli_stmt_init($link);

    try {
        mysqli_stmt_field_count($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (mysqli_stmt_prepare($stmt, ''))
        printf("[004] Prepare should fail for an empty statement\n");

    try {
        mysqli_stmt_field_count($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, 'SELECT 1'))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    if (1 !== ($tmp = mysqli_stmt_field_count($stmt)))
        printf("[007] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, 'SELECT 1, 2'))
        printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    if (2 !== ($tmp = mysqli_stmt_field_count($stmt)))
        printf("[009] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test'))
        printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    if (2 !== ($tmp = mysqli_stmt_field_count($stmt)))
        printf("[011] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, 'SELECT label FROM test') ||
        !mysqli_stmt_execute($stmt))
        printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    if (1 !== ($tmp = mysqli_stmt_field_count($stmt)))
        printf("[013] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    $label = null;
    try {
        if (mysqli_stmt_bind_param($stmt, "s", $label))
            printf("[014] expected error - got ok\n");
    } catch (\ArgumentCountError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    while (mysqli_stmt_fetch($stmt))
        if (1 !== ($tmp = mysqli_stmt_field_count($stmt)))
            printf("[015] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, 'INSERT INTO test(id) VALUES (100)'))
        printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    if (0 !== ($tmp = mysqli_stmt_field_count($stmt)))
        printf("[017] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, "UPDATE test SET label = 'z' WHERE id = 1") ||
        !mysqli_stmt_execute($stmt))
        printf("[018] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (0 !== ($tmp = mysqli_stmt_field_count($stmt)))
        printf("[019] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_prepare($stmt, 'SELECT id FROM test');
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        mysqli_stmt_field_count($stmt);
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
mysqli_stmt object is not fully initialized
The number of variables must match the number of parameters in the prepared statement
mysqli_stmt object is already closed
mysqli_stmt object is already closed
done!
