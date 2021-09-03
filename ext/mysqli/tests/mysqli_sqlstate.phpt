--TEST--
mysqli_sqlstate()
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

    var_dump(mysqli_sqlstate($link));
    mysqli_query($link, "SELECT unknown_column FROM test");
    var_dump(mysqli_sqlstate($link));
    mysqli_free_result(mysqli_query($link, "SELECT id FROM test"));
    var_dump(mysqli_sqlstate($link));

    mysqli_close($link);

    try {
        mysqli_sqlstate($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
%s(5) "00000"
%s(5) "42S22"
%s(5) "00000"
mysqli object is already closed
done!
