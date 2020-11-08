--TEST--
not freed resultset
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /************************
     * non freed resultset
     ************************/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    $result = mysqli_query($link, "SELECT CURRENT_USER()");
    mysqli_close($link);
    printf("Ok\n");

?>
--EXPECT--
Ok
