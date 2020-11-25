--TEST--
Trying implicit reconnect after wait_timeout and KILL using mysqli_ping()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (stristr(mysqli_get_client_info(), 'mysqlnd'))
    die("skip: test for libmysql");
?>
--INI--
mysqli.reconnect=1
--FILE--
<?php
    require_once("connect.inc");
    require_once("table.inc");

    if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot create second database connection, [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());

    $thread_id_timeout = mysqli_thread_id($link);
    $thread_id_control = mysqli_thread_id($link2);

    if (!$res = mysqli_query($link2, "SHOW FULL PROCESSLIST"))
        printf("[002] Cannot get full processlist, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link));

    $running_threads = array();
    while ($row = mysqli_fetch_assoc($res))
        $running_threads[$row['Id']] = $row;
    mysqli_free_result($res);

    if (!isset($running_threads[$thread_id_timeout]) ||
            !isset($running_threads[$thread_id_control]))
        printf("[003] Processlist is borked, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link));

    if (!mysqli_query($link, "SET SESSION wait_timeout = 2"))
        printf("[004] Cannot set wait_timeout, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, "SHOW VARIABLES LIKE 'wait_timeout'"))
        printf("[005] Cannot check if wait_timeout has been set, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[006] Cannot get wait_timeout, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));
    mysqli_free_result($res);

    if ($row['Value'] != 2)
        printf("[007] Failed setting the wait_timeout, test will not work, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    // after 2+ seconds the server should kill the connection
    sleep(3);

    if (!$res = mysqli_query($link2, "SHOW FULL PROCESSLIST"))
        printf("[008] Cannot get full processlist, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link));

    $running_threads = array();
    while ($row = mysqli_fetch_assoc($res))
        $running_threads[$row['Id']] = $row;
    mysqli_free_result($res);

    if (isset($running_threads[$thread_id_timeout]))
        printf("[009] Server should have killed the timeout connection, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link));

    if (true !== mysqli_ping($link))
        printf("[010] Reconnect should have happened");

    if (!$res = mysqli_query($link, "SELECT DATABASE() as _dbname"))
        printf("[011] Cannot get database name, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[012] Cannot get database name, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);
    if ($row['_dbname'] != $db)
        printf("[013] Connection should has been made to DB/Schema '%s', expecting '%s', [%d] %s\n",
            $row['_dbname'], $db, mysqli_errno($link), mysqli_error($link));

    // ... and now we try KILL
    $thread_id_timeout = mysqli_thread_id($link);

    if (!mysqli_query($link2, sprintf('KILL %d', $thread_id_timeout)))
        printf("[014] Cannot KILL timeout connection, [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));
    // Give the server a second to really kill the other thread...
    sleep(1);

    if (!$res = mysqli_query($link2, "SHOW FULL PROCESSLIST"))
        printf("[015] Cannot get full processlist, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link));

    $running_threads = array();
    while ($row = mysqli_fetch_assoc($res))
        $running_threads[$row['Id']] = $row;
    mysqli_free_result($res);

    if (isset($running_threads[$thread_id_timeout]) ||
            !isset($running_threads[$thread_id_control]))
        printf("[016] Processlist is borked, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link));

    if (true !== ($tmp = mysqli_ping($link)))
        printf("[017] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

    if (!$res = mysqli_query($link, "SELECT DATABASE() as _dbname"))
        printf("[018] Cannot get database name, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[019] Cannot get database name, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);
    if ($row['_dbname'] != $db)
        printf("[020] Connection should has been made to DB/Schema '%s', expecting '%s', [%d] %s\n",
            $row['_dbname'], $db, mysqli_errno($link), mysqli_error($link));

    mysqli_close($link);
    mysqli_close($link2);
    print "done!";
?>
--EXPECT--
done!
