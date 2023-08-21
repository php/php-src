--TEST--
mysqli connect
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    $test = "";

    /*** test mysqli_connect localhost:port ***/
    $link = my_mysqli_connect($host, $user, $passwd, "", $port, $socket);
    $test .= ($link) ? "1" : "0";
    mysqli_close($link);

    /*** test mysqli_real_connect ***/
    $link = mysqli_init();
    $test.= (my_mysqli_real_connect($link, $host, $user, $passwd, "", $port, $socket) )
        ? "1" : "0";
    mysqli_close($link);

    /*** test mysqli_real_connect with db ***/
    $link = mysqli_init();
    $test .= (my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket))
        ? "1" : "0";
    mysqli_close($link);

    /*** test mysqli_real_connect with port ***/
    $link = mysqli_init();
    $test .= (my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket))
        ? "1":"0";
    mysqli_close($link);

    /* temporary addition for Kent's setup, Win32 box */
    for ($i = 0; $i < 10; $i++) {
        if (!$link = mysqli_init())
            printf("[001 + %d] mysqli_init() failed, [%d] %s\n", $i, mysqli_connect_errno(), mysqli_connect_error());

        if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket))
            printf("[002 + %d] mysqli_real_connect() failed, [%d] %s\n", $i, mysqli_connect_errno(), mysqli_connect_error());

        mysqli_close($link);
    }

    /*** test mysqli_real_connect compressed ***/
    /*
    $link = mysqli_init();
    $test .= (my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, MYSQLI_CLIENT_COMPRESS))
        ? "1" : "0";
    mysqli_close($link);
  */
    /* todo ssl connections */

    var_dump($test);
    print "done!";
?>
--EXPECTF--
%s(4) "1111"
done!
