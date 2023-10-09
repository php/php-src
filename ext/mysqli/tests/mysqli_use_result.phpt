--TEST--
mysqli_use_result()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_use_result($link)))
        printf("[004] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    try {
        var_dump(mysqli_data_seek($res, 2));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    mysqli_free_result($res);

    if (!mysqli_query($link, "DELETE FROM test"))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (false !== ($res = mysqli_use_result($link)))
        printf("[007] Expecting boolean/false, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id"))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (false !== ($tmp = mysqli_data_seek($res, 1)))
        printf("[009] Expecting boolean/false, got %s/%s\n",
            gettype($tmp), $tmp);

    mysqli_close($link);

    try {
        mysqli_use_result($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
mysqli_data_seek() cannot be used in MYSQLI_USE_RESULT mode
mysqli object is already closed
done!
