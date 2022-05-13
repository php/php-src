--TEST--
mysqli_get_server_version
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

    $i = mysqli_get_server_version($link);

    $test = $i / $i;

    var_dump($test);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
int(1)
done!
