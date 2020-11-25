--TEST--
mysqli_change_user() - ROLLBACK
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
    die(sprintf("skip Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    if (!mysqli_query($link, 'ALTER TABLE test ENGINE=InnoDB'))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_autocommit($link, false);

    if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS _num FROM test'))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    mysqli_free_result($res);

    $num = $row['_num'];
    assert($num > 0);

    if (!$res = mysqli_query($link, 'DELETE FROM test'))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS _num FROM test'))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    mysqli_free_result($res);

    if (0 != $row['_num'])
        printf("[007] Rows should have been deleted in this transaction\n");

    // DELETE should be rolled back
    mysqli_change_user($link, $user, $passwd, $db);

    if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS _num FROM test'))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['_num'] != $num)
        printf("[010] Expecting %d rows in the table test, found %d rows\n",
            $num, $row['_num']);

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
