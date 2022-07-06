--TEST--
mysqli_data_seek()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

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

    try {
        mysqli_data_seek($res, -1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    mysqli_free_result($res);

    if (!$res = mysqli_query($link, 'SELECT * FROM test ORDER BY id', MYSQLI_USE_RESULT))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    try {
        var_dump(mysqli_data_seek($res, 3));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    mysqli_free_result($res);

    try {
        mysqli_data_seek($res, 1);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_data_seek(): Argument #2 ($offset) must be greater than or equal to 0
mysqli_data_seek() cannot be used in MYSQLI_USE_RESULT mode
mysqli_result object is already closed
done!
