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
--EXPECT--
bool(true)
done!
