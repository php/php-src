--TEST--
Bug #51605 Mysqli - zombie links
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.max_links = 1
mysqli.allow_persistent = Off
mysqli.max_persistent = 0
mysqli.reconnect = Off
--FILE--
<?php
    include ("connect.inc");

    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }
    mysqli_close($link);
    echo "closed once\n";

    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }
    mysqli_close($link);
    echo "closed twice\n";

    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        printf("[003] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }
    mysqli_close($link);
    echo "closed for third time\n";

    print "done!";
?>
--EXPECT--
closed once
closed twice
closed for third time
done!
