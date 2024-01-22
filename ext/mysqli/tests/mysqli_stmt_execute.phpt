--TEST--
mysqli_stmt_execute()
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

    // stmt object status test
    try {
        mysqli_stmt_execute($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (mysqli_stmt_prepare($stmt, "SELECT i_do_not_exist_believe_me FROM test ORDER BY id"))
        printf("[005] Statement should have failed!\n");

    // stmt object status test
    try {
        mysqli_stmt_execute($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test ORDER BY id LIMIT 1"))
        printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[008] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
        printf("[009] [%d] %s\n", mysqli_stmt_execute($stmt), mysqli_stmt_execute($stmt));

    // no input variables bound
    if (false !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[010] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    $id = 100;
    $label = "z";
    if (!mysqli_stmt_bind_param($stmt, "is", $id, $label))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    // calling reset between executions
    mysqli_stmt_close($stmt);
    if (!$stmt = mysqli_stmt_init($link))
        printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test ORDER BY id LIMIT ?"))
        printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $limit = 1;
    if (!mysqli_stmt_bind_param($stmt, "i", $limit))
        printf("[015] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[016] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = null;
    if (!mysqli_stmt_bind_result($stmt, $id) || !mysqli_stmt_fetch($stmt))
        printf("[017] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ($id !== 1)
        printf("[018] Expecting int/1 got %s/%s\n", gettype($id), $id);

    if (true !== ($tmp = mysqli_stmt_reset($stmt)))
        printf("[019] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[020] Expecting boolean/true after reset to prepare status, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = null;
    if (!mysqli_stmt_fetch($stmt))
        printf("[021] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ($id !== 1)
        printf("[022] Expecting int/1 got %s/%s\n", gettype($id), $id);

    mysqli_stmt_close($stmt);
    if (!$stmt = mysqli_stmt_init($link))
        printf("[023] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test ORDER BY id LIMIT 1"))
        printf("[024] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[025] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_reset($stmt)))
        printf("[026] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    var_dump(mysqli_stmt_execute($stmt));
    var_dump(mysqli_stmt_fetch($stmt));

    mysqli_kill($link, mysqli_thread_id($link));

    if (false !== ($tmp = mysqli_stmt_execute($stmt)))
        printf("[027] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_execute($stmt);
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
mysqli_stmt object is not fully initialized
mysqli_stmt object is not fully initialized
bool(true)
bool(true)
[027] Expecting boolean/false, got boolean/1
mysqli_stmt object is already closed
done!
