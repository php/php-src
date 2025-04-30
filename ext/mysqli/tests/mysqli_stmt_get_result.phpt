--TEST--
mysqli_stmt_get_result()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    /*
    NOTE: no datatype tests here! This is done by
    mysqli_stmt_bind_result.phpt already. Restrict
    this test case to the basics.
    */
    require 'table.inc';

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    // stmt object status test
    try {
        mysqli_stmt_fetch($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
        printf("[006] Expecting boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (!mysqli_stmt_execute($stmt))
        printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_store_result($stmt))
        printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (is_object($tmp = mysqli_stmt_store_result($stmt)))
        printf("[009] non-object, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    mysqli_stmt_close($stmt);

    if (!$stmt = mysqli_stmt_init($link))
        printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    // stmt object status test
    try {
        mysqli_stmt_fetch($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
        printf("[013] Expecting boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (!mysqli_stmt_execute($stmt))
        printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (! ($tmp = mysqli_stmt_get_result($stmt)) instanceof mysqli_result)
        printf("[016] Expecting mysqli_result, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    mysqli_free_result($tmp);
    mysqli_stmt_close($stmt);

    if (!$stmt = mysqli_stmt_init($link))
        printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    // stmt object status test
    try {
        mysqli_stmt_get_result($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[019] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
        printf("[020] Expecting boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (!mysqli_stmt_execute($stmt))
        printf("[023] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (! ($tmp = mysqli_stmt_get_result($stmt)) instanceof mysqli_result)
        printf("[024] Expecting mysqli_result, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[025] false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (true !== ($tmp = mysqli_stmt_bind_result($stmt, $id, $label))) {
        printf("[026] [%d] [%s]\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
        printf("[027] [%d] [%s]\n", mysqli_errno($link), mysqli_error($link));
        printf("[028] Expecting boolean/true, got %s/%s\n", gettype($tmp), var_export($tmp, true));
    }

    if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[029] false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    mysqli_stmt_close($stmt);

    // get_result can be used in PS cursor mode
    if (!$stmt = mysqli_stmt_init($link))
        printf("[030] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[031] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_attr_set($stmt, MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY))
        printf("[032] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_execute($stmt))
        printf("[033] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $result = mysqli_stmt_get_result($stmt);
    while ($row = mysqli_fetch_assoc($result)) {
        var_dump($row);
    }

    if (!$stmt = mysqli_stmt_init($link))
        printf("[034] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[035] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_execute($stmt))
        printf("[036] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = NULL;
    $label = NULL;
    if (true !== ($tmp = mysqli_stmt_bind_result($stmt, $id, $label)))
        printf("[037] Expecting boolean/true, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (! ($result = mysqli_stmt_get_result($stmt)) instanceof mysqli_result)
        printf("[038] Expecting mysqli_result, got %s/%s, [%d] %s\n",
            gettype($result), var_export($result, true),
            mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[039] Expecting boolean/false, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    printf("[040] [%d] [%s]\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    printf("[041] [%d] [%s]\n", mysqli_errno($link), mysqli_error($link));
    while ($row = mysqli_fetch_assoc($result)) {
        var_dump($row);
    }
    mysqli_free_result($result);

    $link->real_query('KILL '.mysqli_thread_id($link));
    // We kill our own connection so we should get "Query execution was interrupted"
    if ($link->errno !== 1317)
        printf("[042] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
        printf("[043] Expecting boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_fetch($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage(), "\n";
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
mysqli_stmt object is not fully initialized
mysqli_stmt object is not fully initialized
mysqli_stmt object is not fully initialized
array(2) {
  ["id"]=>
  int(1)
  ["label"]=>
  string(1) "a"
}
array(2) {
  ["id"]=>
  int(2)
  ["label"]=>
  string(1) "b"
}
[040] [2014] [Commands out of sync; you can't run this command now]
[041] [0] []
array(2) {
  ["id"]=>
  int(1)
  ["label"]=>
  %s(1) "a"
}
array(2) {
  ["id"]=>
  int(2)
  ["label"]=>
  %s(1) "b"
}
mysqli_stmt object is already closed
done!
