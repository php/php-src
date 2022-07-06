--TEST--
function test: mysqli_stat
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    $status = mysqli_stat($link);

    var_dump(strlen($status) > 0);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
bool(true)
done!
