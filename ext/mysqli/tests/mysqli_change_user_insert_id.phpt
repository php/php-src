--TEST--
mysqli_change_user() - LAST_INSERT_ID() - http://bugs.mysql.com/bug.php?id=45184?
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!$IS_MYSQLND) {
    die("skip Might hit known and open bugs http://bugs.mysql.com/bug.php?id=30472, http://bugs.mysql.com/bug.php?id=45184");
}
?>
--FILE--
<?php
    require_once('connect.inc');

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

    if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, 'CREATE TABLE test(id INT AUTO_INCREMENT PRIMARY KEY, label CHAR(10))'))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, 'z')"))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (($insert_id = mysqli_insert_id($link)) !== 100)
        printf("[005] Expecting 100, got %d, [%d] %s\n",
            $insert_id,
            mysqli_errno($link), mysqli_error($link));

    // LAST_INSERT_ID should be reset
    mysqli_change_user($link, $user, $passwd, $db);

    if (($insert_id = mysqli_insert_id($link)) !== 0)
            printf("[006] Expecting 0, got %d, [%d] %s\n",
                $insert_id,
                mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, 'SELECT LAST_INSERT_ID() as _insert_id'))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    mysqli_free_result($res);

    if ($row['_insert_id'] != $insert_id)
        printf("LAST_INSERT_ID() [%d] and mysqli_insert_id [%d] differ!\n",
            $row['_insert_id'], $insert_id);

    if ($row['_insert_id'] != 0)
        printf("Expecting 0 got %d\n", $row['_insert_id']);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
