--TEST--
mysqli_change_user() - table locks, GET_LOCK(), temporary tables
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
die("skip - is the server still buggy?");
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot create second connection handle, [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());

    if (!mysqli_query($link, 'LOCK TABLE test WRITE'))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    /*
    if ($res = mysqli_query($link2, 'SELECT COUNT(*) AS _num FROM test')) {
        printf("[003] Reading from test should not be possible due to a lock, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link2));
        mysqli_free_result($res);
    }
    */

    // LOCKS should be removed
    mysqli_change_user($link, $user, $passwd, $db);

    if (!$res = mysqli_query($link2, 'SELECT COUNT(*) AS _num FROM test'))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_num'] < 1)
        printf("[005] There should be some rows in the table test\n");

    mysqli_free_result($res);
    mysqli_close($link2);

    if (!mysqli_query($link, 'DROP TABLE test'))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, 'CREATE TEMPORARY TABLE test(id INT)'))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1), (2), (3)'))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS _num FROM test'))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_num'] != 3)
        printf("[011] There should be three rows in the table test\n");

    mysqli_free_result($res);

    // Temporary tables should be dropped
    mysqli_change_user($link, $user, $passwd, $db);

    if ($res = mysqli_query($link, 'SELECT COUNT(*) AS _num FROM test')) {
        printf("[012] There should be no table test any more, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));
        mysqli_free_result($res);
    }

    if (!$res = mysqli_query($link, 'SELECT GET_LOCK("phptest", 2) AS _ok'))
        printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_ok'] != 1)
        printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);

    // GET_LOCK("phptest") should be released
    mysqli_change_user($link, $user, $passwd, $db);

    if (!$res = mysqli_query($link, 'SELECT IS_FREE_LOCK("phptest") AS _ok'))
        printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_ok'] != 1)
        printf("[018] Lock 'phptest' should have been released, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
