--TEST--
mysqli_thread_id()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if (!is_int($tmp = mysqli_thread_id($link)) || (0 === $tmp))
        printf("[003] Expecting int/any but zero, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    // should work if the thread id is correct
    mysqli_kill($link, mysqli_thread_id($link));

    mysqli_close($link);

    try {
        mysqli_thread_id($link);
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
