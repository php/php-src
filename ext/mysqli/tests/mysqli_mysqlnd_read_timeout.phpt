--TEST--
mysqlnd.net_read_timeout limit check
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$IS_MYSQLND)
    /* The libmysql read_timeout limit default is 365 * 24 * 3600 seconds. It cannot be altered through PHP API calls */
    die("skip mysqlnd only test");
?>
--INI--
default_socket_timeout=60
max_execution_time=60
mysqlnd.net_read_timeout=1
--FILE--
<?php
    include ("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    if (!$res = mysqli_query($link, "SELECT SLEEP(5)"))
        printf("[002] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

    mysqli_close($link);

    print "done!";
?>
--EXPECT--
[002] [2006] MySQL server has gone away
done!
