--TEST--
mysqli_change_user() - GET_LOCK()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
die("skip - is the server still buggy?");
?>
--INI--
max_execution_time=240
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    // We need this little hack to be able to re-run the test
    $lock = 'phptest_' . mt_rand(0, 100000);
    $thread_id = mysqli_thread_id($link);

    printf("Testing GET_LOCK()...\n");

    if (!$res = mysqli_query($link, sprintf('SELECT GET_LOCK("%s", 2) AS _ok', $lock)))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_ok'] != 1)
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    else
        printf("... lock '%s' acquired by thread %d\n", $lock, $thread_id);

    mysqli_free_result($res);


    // GET_LOCK("phptest") should be released
    /* From the mysql_change_user documentation:
This command resets the state as if one had done a new connect. (See Section 25.2.13, “Controlling Automatic Reconnect Behavior”.) It always performs a ROLLBACK of any active transactions, closes and drops all temporary tables, and unlocks all locked tables. Session system variables are reset to the values of the corresponding global system variables. Prepared statements are released and HANDLER variables are closed. Locks acquired with GET_LOCK() are released. These effects occur even if the user didn't change.
    */
    mysqli_change_user($link, $user, $passwd, $db);
sleep(5);
    $new_thread_id = mysqli_thread_id($link);

    printf("... calling IS_USED_LOCK() on '%s' using thread '%d'\n", $lock, $new_thread_id);
    if (!$res = mysqli_query($link, 'SELECT IS_USED_LOCK("phptest") AS _ok'))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_ok'] != NULL)
        printf("[006] Lock '%s' should have been released, [%d] %s\n",
            $lock,
            mysqli_errno($link), mysqli_error($link));

    printf("... calling IS_FREE_LOCK() on '%s' using thread '%d'\n", $lock, $new_thread_id);
    if (!$res = mysqli_query($link, 'SELECT IS_FREE_LOCK("phptest") AS _ok'))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_ok'] != 1)
        printf("[009] Lock '%s' should have been released, [%d] %s\n",
            $lock,
            mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);

    /* Ok, let's try a NEW connection and a NEW lock! */
    mysqli_close($link);
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[010] Cannot open new connection, [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());

    do {
        $newlock = 'phptest_' . mt_rand(0, 100000);
    } while ($lock == $newlock);

    $new_thread_id = mysqli_thread_id($link);
    printf("... calling IS_USED_LOCK() on '%s' using new connection with thread '%d'\n", $newlock, $new_thread_id);
    if (!$res = mysqli_query($link, 'SELECT IS_USED_LOCK("phptest") AS _ok'))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_ok'] != NULL)
        printf("[013] Lock '%s' should have been released, [%d] %s\n",
            $lock,
            mysqli_errno($link), mysqli_error($link));

    print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Testing GET_LOCK()...
... lock 'phptest_%d' acquired by thread %d
... calling IS_USED_LOCK() on 'phptest_%d' using thread '%d'
... calling IS_FREE_LOCK() on 'phptest_%d' using thread '%d'
... calling IS_USED_LOCK() on 'phptest_%d' using new connection with thread '%d'
done!
