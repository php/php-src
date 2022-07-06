--TEST--
mysqli_stat()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if ((!is_string($tmp = mysqli_stat($link))) || ('' === $tmp))
        printf("[004] Expecting non empty string, got %s/'%s', [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysql_error($link));

    mysqli_close($link);

    try {
        mysqli_stat($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
