--TEST--
mysqli_stmt_close()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    try {
        mysqli_stmt_close($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test"))
        printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_close($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!$stmt = mysqli_prepare($link, "INSERT INTO test(id, label) VALUES (?, ?)"))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $id = $label = null;
    if (!mysqli_stmt_bind_param($stmt, "is", $id, $label))
        printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = 100; $label = 'z';
    if (!mysqli_stmt_execute($stmt))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $link->query('KILL '.mysqli_thread_id($link));

    mysqli_stmt_close($stmt);

    mysqli_close($link);

    require 'table.inc';
    if (!$stmt = mysqli_prepare($link, "SELECT id, label FROM test"))
        printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $id = $label = null;
    if (!mysqli_stmt_bind_result($stmt, $id, $label))
        printf("[015] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_execute($stmt) || !mysqli_stmt_fetch($stmt))
        printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $link->query('KILL '.mysqli_thread_id($link));

    mysqli_stmt_close($stmt);

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
Deprecated: Function mysqli_stmt_init() is deprecated since 8.6, use mysqli_prepare() instead in %s on line %d
mysqli_stmt object is not fully initialized
mysqli_stmt object is already closed
done!
