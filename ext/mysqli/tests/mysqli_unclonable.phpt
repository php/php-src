--TEST--
Trying to clone mysqli object
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    $link_clone = clone $link;
    mysqli_close($link);

    print "done!";
?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class mysqli in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
