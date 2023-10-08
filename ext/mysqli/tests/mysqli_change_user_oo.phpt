--TEST--
mysqli->change_user()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';

require_once 'table.inc';

if (mysqli_get_server_version($link) >= 50600)
    die("SKIP For MySQL < 5.6.0");
?>
--FILE--
<?php
    require_once 'connect.inc';

    $link	= NULL;
    $tmp	= NULL;

    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    if (false !== ($tmp = $mysqli->change_user($user . '_unknown_really', $passwd . 'non_empty', $db)))
        printf("[006] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = $mysqli->change_user($user, $passwd . '_unknown_really', $db)))
        printf("[007] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = $mysqli->change_user($user, $passwd, $db . '_unknown_really')))
        printf("[008] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    // Reconnect because after 3 failed change_user attempts, the server blocks you off.
    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    if (!$mysqli->query('SET @mysqli_change_user_test_var=1'))
        printf("[009] Failed to set test variable: [%d] %s\n", $mysqli->errno, $mysqli->error);

    if (!$res = $mysqli->query('SELECT @mysqli_change_user_test_var AS test_var'))
        printf("[010] [%d] %s\n", $mysqli->errno, $mysqli->error);
    $tmp = $res->fetch_assoc();
    $res->free_result();
    if (1 != $tmp['test_var'])
        printf("[011] Cannot set test variable\n");

    if (true !== ($tmp = $mysqli->change_user($user, $passwd, $db)))
        printf("[012] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

    if (!$res = $mysqli->query('SELECT database() AS dbname, user() AS user'))
        printf("[013] [%d] %s\n", $mysqli->errno, $mysqli->error);
    $tmp = $res->fetch_assoc();
    $res->free_result();

    if (substr($tmp['user'], 0, strlen($user)) !== $user)
        printf("[014] Expecting user %s, got user() %s\n", $user, $tmp['user']);
    if ($tmp['dbname'] != $db)
        printf("[015] Expecting database %s, got database() %s\n", $db, $tmp['dbname']);

    if (!$res = $mysqli->query('SELECT @mysqli_change_user_test_var AS test_var'))
        printf("[016] [%d] %s\n", $mysqli->errno, $mysqli->error);
    $tmp = $res->fetch_assoc();
    $res->free_result();
    if (NULL !== $tmp['test_var'])
        printf("[017] Test variable is still set!\n");

    $mysqli->close();

    if (false !== ($tmp = @$mysqli->change_user($user, $passwd, $db)))
        printf("[018] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--EXPECT--
done!
