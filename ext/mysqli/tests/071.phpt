--TEST--
mysqli thread_id & kill
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

    var_dump($mysql->query('DO 1'));

    $ret = $mysql->kill($mysql->thread_id);
    if ($ret !== true){
        printf("[001] Expecting boolean/true got %s/%s\n", gettype($ret), var_export($ret, true));
    }

    var_dump($mysql->query('DO 1'));

    $mysql->close();

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    var_dump($mysql->query('DO 1'));

    $ret = $mysql->kill($mysql->thread_id);
    if ($ret !== true){
        printf("[002] Expecting boolean/true got %s/%s\n", gettype($ret), var_export($ret, true));
    }

    var_dump($mysql->query('DO 1'));

    $mysql->close();
    print "done!";
?>
--EXPECTF--
bool(true)

Deprecated: Method mysqli::kill() is deprecated since 8.4, use KILL CONNECTION/QUERY SQL statement instead in %s
bool(false)
bool(true)

Deprecated: Method mysqli::kill() is deprecated since 8.4, use KILL CONNECTION/QUERY SQL statement instead in %s
bool(false)
done!
