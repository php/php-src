--TEST--
mysqli_query() - unicode (cyrillic)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
require_once('table.inc');
if (!$res = mysqli_query($link, "SHOW CHARACTER SET LIKE 'utf8'"))
    die("skip UTF8 chatset seems not available");
mysqli_free_result($res);
mysqli_close($link);
?>
--FILE--
<?php
    include_once("connect.inc");

    require_once('table.inc');

    if (TRUE !== ($tmp = @mysqli_query($link, "set names utf8")))
        printf("[002.5] Expecting TRUE, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = mysqli_query($link, 'това не е ескюел')))
        printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = mysqli_query($link, "SELECT 'това е ескюел, но със обратна наклонена и g'\g")))
        printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if ((0 === mysqli_errno($link)) || ('' == mysqli_error($link)))
        printf("[006] mysqli_errno()/mysqli_error should return some error\n");

    if (!$res = mysqli_query($link, "SELECT 'това ескюел, но с точка и запетая' AS правилен ; "))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump(mysqli_fetch_assoc($res));
    mysqli_free_result($res);

    if (false !== ($res = mysqli_query($link, "SELECT 'това ескюел, но с точка и запетая' AS правилен ; SHOW VARIABLES")))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_get_server_version($link) > 50000) {
        // let's try to play with stored procedures
        mysqli_query($link, 'DROP PROCEDURE IF EXISTS процедурка');
        if (mysqli_query($link, 'CREATE PROCEDURE процедурка(OUT версия VARCHAR(25)) BEGIN SELECT VERSION() INTO версия; END;')) {
            $res = mysqli_query($link, 'CALL процедурка(@version)');
            $res = mysqli_query($link, 'SELECT @version AS п_версия');

            $tmp = mysqli_fetch_assoc($res);
            if (!is_array($tmp) || empty($tmp) || !isset($tmp['п_версия']) || ('' == $tmp['п_версия'])) {
                printf("[008a] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                var_dump($tmp);
            }

            mysqli_free_result($res);
        } else {
            printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        }

        mysqli_query($link, 'DROP FUNCTION IF EXISTS функцийка');
        if (mysqli_query($link, 'CREATE FUNCTION функцийка( параметър_версия VARCHAR(25)) RETURNS VARCHAR(25) DETERMINISTIC RETURN параметър_версия;')) {
            $res = mysqli_query($link, 'SELECT функцийка(VERSION()) AS ф_версия');

            $tmp = mysqli_fetch_assoc($res);
            if (!is_array($tmp) || empty($tmp) || !isset($tmp['ф_версия']) || ('' == $tmp['ф_версия'])) {
                printf("[009a] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                var_dump($tmp);
            }

            mysqli_free_result($res);
        } else {
            printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        }
    }

    mysqli_close($link);

    try {
        mysqli_query($link, "SELECT id FROM test");
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECTF--
array(1) {
  ["правилен"]=>
  string(%d) "това ескюел, но с точка и запетая"
}
mysqli object is already closed
done!
