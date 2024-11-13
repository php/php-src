--TEST--
mysqli ping
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
    var_dump($mysql->ping());
    $mysql->close();
    print "done!";
?>
--EXPECTF--

Deprecated: Method mysqli::ping() is deprecated since 8.4, because the reconnect feature has been removed in PHP 8.2 and this method is now redundant in %s
bool(true)
done!
