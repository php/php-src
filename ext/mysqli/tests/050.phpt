--TEST--
non freed statement test
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /************************
     * non freed stamement
     ************************/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    $stmt = mysqli_prepare($link, "SELECT CURRENT_USER()");
    mysqli_stmt_execute($stmt);

    mysqli_close($link);
    printf("Ok\n");
?>
--EXPECT--
Ok
