--TEST--
mysqli_change_user()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[006] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (false !== ($tmp = @mysqli_change_user($link, $user . '_unknown_really', $passwd . 'non_empty', $db)))
        printf("[007] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[008] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (false !== ($tmp = @mysqli_change_user($link, $user, $passwd . '_unknown_really', $db)))
        printf("[009] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    // Reconnect because after 3 failed change_user attempts, the server blocks you off.
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[009a] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!mysqli_query($link, 'SET @mysqli_change_user_test_var=1'))
        printf("[010] Failed to set test variable: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, 'SELECT @mysqli_change_user_test_var AS test_var'))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (1 != $tmp['test_var'])
        printf("[012] Cannot set test variable\n");

    if (true !== ($tmp = mysqli_change_user($link, $user, $passwd, $db)))
        printf("[013] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

    if (!$res = mysqli_query($link, 'SELECT database() AS dbname, user() AS user'))
        printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);

    if (substr($tmp['user'], 0, strlen($user)) !== $user)
        printf("[015] Expecting user %s, got user() %s\n", $user, $tmp['user']);
    if ($tmp['dbname'] != $db)
        printf("[016] Expecting database %s, got database() %s\n", $db, $tmp['dbname']);

    if (false !== ($tmp = @mysqli_change_user($link, $user, $passwd, $db . '_unknown_really')))
        printf("[011] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[012] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    if (false !== ($tmp = @mysqli_change_user($link, str_repeat('user', 16384), str_repeat('pass', 16384), str_repeat('dbase', 16384))))
        printf("[013] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[014] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    /* silent protocol change if no db which requires workaround in mysqlnd/libmysql
    (empty db = no db send with COM_CHANGE_USER) */
    if (true !== ($tmp = mysqli_change_user($link, $user, $passwd, "")))
        printf("[015] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

    if (!$res = mysqli_query($link, 'SELECT database() AS dbname, user() AS user'))
        printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);

    if ($tmp['dbname'] != "")
        printf("[017] Expecting database '', got database() '%s'\n", $tmp['dbname']);

    mysqli_close($link);

    try {
        mysqli_change_user($link, $user, $passwd, $db);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[019] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!mysqli_query($link, 'SET @mysqli_change_user_test_var=1'))
        printf("[020] Failed to set test variable: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, 'SELECT @mysqli_change_user_test_var AS test_var'))
        printf("[021] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (1 != $tmp['test_var'])
        printf("[022] Cannot set test variable\n");

    if (true !== ($tmp = mysqli_change_user($link, $user, $passwd, $db)))
        printf("[023] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

    if (!$res = mysqli_query($link, 'SELECT database() AS dbname, user() AS user'))
        printf("[024] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);

    if (substr($tmp['user'], 0, strlen($user)) !== $user)
        printf("[025] Expecting user %s, got user() %s\n", $user, $tmp['user']);
    if ($tmp['dbname'] != $db)
        printf("[026] Expecting database %s, got database() %s\n", $db, $tmp['dbname']);

    if (!$res = mysqli_query($link, 'SELECT @mysqli_change_user_test_var AS test_var'))
        printf("[027] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (NULL !== $tmp['test_var'])
        printf("[028] Test variable is still set!\n");

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
