--TEST--
mysqli_stmt_get_result()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_stmt_get_result'))
    die('skip mysqli_stmt_get_result not available');
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

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 1"))
        printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!mysqli_stmt_execute($stmt))
            printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
            printf("[007] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
                gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
        }
        while ($row = mysqli_fetch_assoc($res))
            var_dump($row);
        var_dump(mysqli_fetch_assoc($res));
        mysqli_free_result($res);

        if (false !== ($res = mysqli_stmt_get_result($stmt))) {
            printf("[008] boolean/false got %s/%s, [%d] %s\n",
                gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
        }

        mysqli_stmt_execute($stmt);
        if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
            printf("[009] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
                gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
        }
        while ($row = mysqli_fetch_assoc($res))
            var_dump($row);
        var_dump(mysqli_fetch_assoc($res));
        mysqli_free_result($res);

    mysqli_stmt_close($stmt);

    if (!($stmt = mysqli_stmt_init($link)) ||
        !mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2") ||
        !mysqli_stmt_execute($stmt))
        printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = $label = null;
    if (!mysqli_stmt_bind_result($stmt, $id, $label))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_fetch($stmt))
        printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (false !== ($res = mysqli_stmt_get_result($stmt))) {
        printf("[013] boolean/false got %s/%s, [%d] %s\n",
            gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    mysqli_stmt_close($stmt);

    if (!($stmt = mysqli_stmt_init($link)) ||
        !mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2") ||
        !mysqli_stmt_execute($stmt))
        printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
        printf("[015] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
            gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    $id = $label = null;
    if (!mysqli_stmt_bind_result($stmt, $id, $label))
        printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_fetch($stmt))
        printf("[017] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    mysqli_stmt_close($stmt);

    if (!($stmt = mysqli_stmt_init($link)) ||
        !mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2") ||
        !mysqli_stmt_execute($stmt))
        printf("[018] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
        printf("[019] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
            gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    $id = $label = null;
    if (!mysqli_stmt_bind_result($stmt, $id, $label))
        printf("[020] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $row = mysqli_fetch_assoc($res);
    if (NULL !== $id || NULL !== $label)
        printf("[021] Bound variables should not have been set\n");
    mysqli_free_result($res);

    mysqli_stmt_close($stmt);

    if (!($stmt = mysqli_stmt_init($link)) ||
        !mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2") ||
        !mysqli_stmt_execute($stmt))
        printf("[022] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
        printf("[023] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
            gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }
    if (!in_array($res->type, array(MYSQLI_STORE_RESULT, MYSQLI_USE_RESULT))) {
        printf("[024] Unknown result set type %s\n", $res->type);
    }
    if ($res->type !== MYSQLI_STORE_RESULT)
        printf("[025] Expecting int/%d got %s/%s", MYSQLI_STORE_RESULT, gettype($res->type), $res->type);

    mysqli_free_result($res);
    mysqli_stmt_close($stmt);
    mysqli_close($link);

    try {
        mysqli_stmt_get_result($stmt);
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
array(2) {
  ["id"]=>
  int(1)
  ["label"]=>
  string(1) "a"
}
NULL
array(2) {
  ["id"]=>
  int(1)
  ["label"]=>
  string(1) "a"
}
NULL
[017] [2014] Commands out of sync; you can't run this command now
mysqli_stmt object is already closed
done!
