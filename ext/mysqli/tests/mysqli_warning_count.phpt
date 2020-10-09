--TEST--
mysqli_warning_count()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if (!$res = mysqli_query($link, "SELECT id, label FROM test"))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (0 !== ($tmp = mysqli_warning_count($link)))
        printf("[005] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_query($link, "DROP TABLE IF EXISTS this_table_does_not_exist"))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (1 !== ($tmp = mysqli_warning_count($link)))
        printf("[007] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    try {
        mysqli_warning_count($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli object is already closed
done!
