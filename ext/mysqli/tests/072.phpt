--TEST--
mysqli warning_count, get_warnings
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

    $mysql->query("DROP TABLE IF EXISTS not_exists");

    var_dump($mysql->warning_count);

    $w = $mysql->get_warnings();

    var_dump($w->errno);
    var_dump($w->message);
    var_dump($w->sqlstate);

    $mysql->close();
    echo "done!"
?>
--EXPECTF--
int(1)
int(1051)
string(%d) "Unknown table %snot_exists%s"
string(5) "HY000"
done!
