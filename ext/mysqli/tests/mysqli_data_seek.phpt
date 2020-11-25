--TEST--
mysqli_data_seek()
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

    if (NULL !== ($tmp = @mysqli_data_seek()))
        printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (NULL !== ($tmp = @mysqli_data_seek($link)))
        printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (NULL !== ($tmp = @mysqli_data_seek($link, $link)))
        printf("[003] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');
    if (!$res = mysqli_query($link, 'SELECT * FROM test ORDER BY id LIMIT 4', MYSQLI_STORE_RESULT))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = mysqli_data_seek($res, 3)))
        printf("[005] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    $row = mysqli_fetch_assoc($res);
    if (4 != $row['id'])
        printf("[006] Expecting record 4/d, got record %s/%s\n", $row['id'], $row['label']);

    if (true !== ($tmp = mysqli_data_seek($res, 0)))
        printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    $row = mysqli_fetch_assoc($res);
    if (1 != $row['id'])
        printf("[008] Expecting record 1/a, got record %s/%s\n", $row['id'], $row['label']);

    if (false !== ($tmp = mysqli_data_seek($res, 4)))
        printf("[009] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = mysqli_data_seek($res, -1)))
        printf("[010] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_free_result($res);

    if (!$res = mysqli_query($link, 'SELECT * FROM test ORDER BY id', MYSQLI_USE_RESULT))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (false !== ($tmp = mysqli_data_seek($res, 3)))
        printf("[012] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_free_result($res);

    if (false !== ($tmp = mysqli_data_seek($res, 1)))
        printf("[013] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_data_seek(): Function cannot be used with MYSQL_USE_RESULT in %s on line %d

Warning: mysqli_data_seek(): Couldn't fetch mysqli_result in %s on line %d
done!
