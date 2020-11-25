--TEST--
mysqli_stmt_fetch()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
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

    $tmp    = NULL;
    $link   = NULL;

    if (!is_null($tmp = @mysqli_stmt_fetch()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_stmt_fetch($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    // stmt object status test
    if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[004] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    // FIXME - different versions return different values ?!
    if ((NULL !== ($tmp = mysqli_stmt_fetch($stmt))) && (false !== $tmp))
        printf("[006] Expecting NULL or boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_execute($stmt))
        printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (true !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[008] NULL, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    if (!$stmt = mysqli_stmt_init($link))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
        printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_execute($stmt))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = NULL;
    $label = NULL;
    if (true !== ($tmp = mysqli_stmt_bind_result($stmt, $id, $label)))
        printf("[012] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[013] Expecting boolean/true, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_kill($link, mysqli_thread_id($link)))
        printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[015] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);

    if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
        printf("[016] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    /* Check that the function alias exists. It's a deprecated function,
    but we have not announce the removal so far, therefore we need to check for it */
    if (!is_null($tmp = @mysqli_stmt_fetch()))
        printf("[017] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_fetch(): invalid object or resource mysqli_stmt
 in %s on line %d
[014] [%d] Commands out of sync; you can't run this command now

Warning: mysqli_stmt_fetch(): Couldn't fetch mysqli_stmt in %s on line %d
done!
