--TEST--
function test: mysqli_error
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
    $error = mysqli_error($link);
    var_dump($error);

    mysqli_select_db($link, $db);

    mysqli_query($link, "SELECT * FROM non_exisiting_table");
    $error = mysqli_error($link);

    var_dump($error);

    mysqli_close($link);
    print "done!";
?>
--EXPECTF--
string(0) ""
string(%d) "%s"
done!
