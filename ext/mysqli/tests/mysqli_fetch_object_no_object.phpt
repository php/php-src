--TEST--
mysqli_fetch_object()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require('table.inc');
    if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 5")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    $obj = mysqli_fetch_object($res);
    var_dump(gettype($obj));
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
%s(6) "object"
done!
