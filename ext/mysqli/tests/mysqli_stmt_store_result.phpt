--TEST--
mysqli_stmt_store_result()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    if (!is_null($tmp = @mysqli_stmt_store_result()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_stmt_store_result($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');

    if (false !== ($tmp = @mysqli_stmt_store_result(new mysqli_stmt())))
        printf("[003] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (!$stmt = mysqli_stmt_init($link))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    // stmt object status test
    if (false !== ($tmp = @mysqli_stmt_store_result($stmt)))
        printf("[005] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (100, 'z')") ||
        !mysqli_stmt_execute($stmt))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = @mysqli_stmt_store_result($stmt)))
        printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test ORDER BY id') ||
        !mysqli_stmt_execute($stmt))
        printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!$link_buf = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[009] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

    if (!$stmt_buf = mysqli_stmt_init($link_buf))
        printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt_buf, "SELECT id, label FROM test ORDER BY id") ||
        !mysqli_stmt_execute($stmt_buf))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt_buf), mysqli_stmt_error($stmt_buf));

    $id = $label = $id_buf = $label_buf = null;
    if (!mysqli_stmt_bind_result($stmt, $id, $label))
        printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_bind_result($stmt_buf, $id_buf, $label_buf))
        printf("[013] [%d] %s\n", mysqli_stmt_errno($stmt_buf), mysqli_stmt_error($stmt_buf));

    while (mysqli_stmt_fetch($stmt)) {
        if (!mysqli_stmt_fetch($stmt_buf)) {
            printf("[014] Unbuffered statement indicates more rows than buffered, [%d] %s\n",
                mysqli_stmt_errno($stmt_buf), mysqli_stmt_error($stmt_buf));
        }
        if ($id !== $id_buf)
            printf("[015] unbuffered '%s'/%s, buffered '%s'/%s\n",
                $id, gettype($id), $id_buf, gettype($id_buf));
        if ($label !== $label_buf)
            printf("[016] unbuffered '%s'/%s, buffered '%s'/%s\n",
                $label, gettype($label), $label_buf, gettype($label_buf));
    }

    mysqli_stmt_close($stmt);
    mysqli_stmt_close($stmt_buf);

    if (false !== ($tmp = @mysqli_stmt_store_result($stmt)))
        printf("[017] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);
    mysqli_close($link_buf);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
