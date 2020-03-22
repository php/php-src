--TEST--
mysqli_pconnect()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $host = 'p:' . $host;
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    mysqli_close($link);

    $num = 20;
    $connections = array();
    for ($i = 0; $i < $num; $i++) {
        if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
            printf("[003] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        $connections[] = $link;
    }
    while (count($connections)) {
        do {
            $index = mt_rand(0, $num);
        } while (!isset($connections[$index]));
        mysqli_close($connections[$index]);
        unset($connections[$index]);
    }


    $connections = array();
    $num = 20;
    for ($i = 0; $i < $num; $i++) {
        if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
            printf("[004] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        $connections[] = $link;
    }
    $left = $num;

    while (count($connections) && $left > 0) {
        do {
            $index = mt_rand(0, $num);
        } while (!isset($connections[$index]) && $left > 0);
        if (mt_rand(0, 1) > 0) {
            $left--;
            mysqli_close($connections[$index]);
            unset($connections[$index]);
        } else {
            $left--;
            if (!$connections[$index] = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
                printf("[004] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        }
        flush();
    }

    while (count($connections)) {
        do {
            $index = mt_rand(0, $num);
        } while (!isset($connections[$index]));
        mysqli_close($connections[$index]);
        unset($connections[$index]);
    }

    print "done!";
?>
--EXPECT--
done!
